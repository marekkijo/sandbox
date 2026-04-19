#include "decoder.hpp"

#include "streaming_common/constants.hpp"

#include <stdexcept>

namespace streaming {
void Decoder::init(const VideoStreamInfo &video_stream_info) {
  if (rgb_frame_) {
    throw std::runtime_error{"Decoder is already initialized"};
  }

  rgb_frame_ = std::make_shared<FrameData>(video_stream_info.width * video_stream_info.height * CHANNELS_NUM);
  codec_ = avcodec_find_decoder(video_stream_info.codec_id);
  context_.reset(codec_ ? avcodec_alloc_context3(codec_) : nullptr);
  parser_.reset(codec_ ? av_parser_init(codec_->id) : nullptr);
  packet_.reset(av_packet_alloc());
  if (codec_ == nullptr) {
    throw std::runtime_error{"avcodec_find_decoder failed"};
  }
  if (!context_) {
    throw std::runtime_error{"avcodec_alloc_context3 failed"};
  }
  if (!parser_) {
    throw std::runtime_error{"av_parser_init failed"};
  }
  if (!packet_) {
    throw std::runtime_error{"av_packet_alloc failed"};
  }

  context_->width = video_stream_info.width;
  context_->height = video_stream_info.height;
  context_->thread_count = DECODE_THREAD_COUNT;

  if (avcodec_open2(context_.get(), codec_, nullptr) < 0) {
    throw std::runtime_error{"avcodec_open2 failed"};
  }

  frame_.reset(av_frame_alloc());
  if (!frame_) {
    throw std::runtime_error{"av_frame_alloc failed"};
  }

  packet_sent_ = false;
  {
    buffer_.clear();
    signaled_eof_ = false;
  }
}

Decoder::~Decoder() = default;

std::shared_ptr<FrameData> Decoder::rgb_frame() {
  if (!rgb_frame_) {
    throw std::runtime_error{"Decoder is not initialized"};
  }

  return rgb_frame_;
}

Decoder::Status Decoder::decode() {
  if (!rgb_frame_) {
    throw std::runtime_error{"Decoder is not initialized"};
  }

  if (!context_) {
    return {Status::Code::EOS};
  }

  if (packet_sent_) {
    auto result = avcodec_receive_frame(context_.get(), frame_.get());

    if (result == AVERROR(EAGAIN)) {
      packet_sent_ = false;
      return decode();
    }
    if (result == AVERROR_EOF) {
      destroy();
      return {Status::Code::EOS};
    }
    if (result < 0) {
      return {Status::Code::ERROR};
    }

    // context_ is guaranteed non-null here: destroy() returns EOS above
    const auto frame_num = static_cast<int>(context_->frame_num);
    yuv_to_rgb();
    return {Status::Code::OK, frame_num};
  } else {
    if (upload()) {
      return {Status::Code::RETRY};
    } else {
      return {Status::Code::NODATA};
    }
  }
}

bool Decoder::incoming_data(const std::byte *data, const std::size_t size, const bool async) {
  if (!async && !rgb_frame_) {
    throw std::runtime_error{"Decoder is already initialized"};
  }

  if (signaled_eof_) {
    return false;
  }

  if (async) {
    fill_async_buffer(data, size);
    return true;
  }

  if (buffer_.empty()) {
    buffer_.reserve(size + NULL_PADDING.size());
  } else {
    // Only strip the trailing null-padding if it actually fits in the buffer.
    if (buffer_.size() >= NULL_PADDING.size()) {
      buffer_.erase(buffer_.begin() + static_cast<std::int64_t>(buffer_.size()) - NULL_PADDING.size(), buffer_.end());
    } else {
      buffer_.clear();
    }
    buffer_.reserve(buffer_.size() + size + NULL_PADDING.size());
  }

  buffer_.insert(buffer_.end(),
                 reinterpret_cast<const std::uint8_t *>(data),
                 reinterpret_cast<const std::uint8_t *>(data + size));
  buffer_.insert(buffer_.end(), NULL_PADDING.begin(), NULL_PADDING.end());

  return true;
}

void Decoder::signal_eof() { signaled_eof_ = true; }

bool Decoder::upload() {
  consume_async_buffer();

  // Iterative loop to avoid unbounded recursion on zero-size packets (e.g.
  // parser header/padding packets that produce no output data).
  while (true) {
    const auto eof = buffer_.empty() && signaled_eof_;
    while (!buffer_.empty() || eof) {
      const auto size = buffer_.empty() ? 0u : (buffer_.size() - NULL_PADDING.size());
      auto result = av_parser_parse2(parser_.get(),
                                     context_.get(),
                                     &packet_->data,
                                     &packet_->size,
                                     buffer_.data(),
                                     static_cast<int>(size),
                                     AV_NOPTS_VALUE,
                                     AV_NOPTS_VALUE,
                                     0);

      auto used = result;

      if (packet_->size != 0) {
        result = avcodec_send_packet(context_.get(), packet_.get());
        if (result == 0) {
          reduce_buffer(used);
          packet_sent_ = true;
          return true;
        }
        if (result == AVERROR(EAGAIN)) {
          reduce_buffer(used);
          packet_sent_ = true;
          return true;
        }
        if (result == AVERROR_EOF) {
          printf("AVERROR_EOF: the decoder has been flushed, and no new packets can be sent to it (also returned if more "
                 "than 1 flush packet is sent)");
          return false;
        }
        if (result == AVERROR_INVALIDDATA) {
          printf("AVERROR_INVALIDDATA: invalid data found when processing input\n");
          reduce_buffer(used);
          return false;
        }
        if (result == AVERROR(EINVAL)) {
          throw std::runtime_error("AVERROR(EINVAL): codec not opened, it is an encoder, or requires flush");
        }
        if (result == AVERROR(ENOMEM)) {
          throw std::runtime_error("AVERROR(ENOMEM): failed to add packet to internal queue, or similar other errors: "
                                   "legitimate decoding errors");
        }
        throw std::runtime_error("avcodec_send_packet filed with error: " + std::to_string(result));
      } else if (eof) {
        break;
      } else {
        // Parser consumed bytes but produced no packet (header/padding); reduce
        // the buffer and try again in the same iteration instead of recursing.
        reduce_buffer(used);
        continue;
      }
    }

    if (signaled_eof_) {
      auto result = avcodec_send_packet(context_.get(), nullptr);
      if (result == 0 || result == AVERROR_EOF) {
        packet_sent_ = true;
        return true;
      } else {
        throw std::runtime_error("avcodec_send_packet EOF failed");
      }
    }

    return false;
  }
}

void Decoder::reduce_buffer(int n) {
  if (buffer_.empty()) {
    return;
  }

  if (static_cast<std::size_t>(n) == (buffer_.size() - NULL_PADDING.size())) {
    buffer_.clear();
  } else {
    buffer_.erase(buffer_.begin(), buffer_.begin() + n);
  }
}

void Decoder::destroy() {
  sws_context_.reset();
  frame_.reset();
  packet_.reset();
  parser_.reset();
  context_.reset();
  codec_ = nullptr;
}

void Decoder::yuv_to_rgb() {
  const std::array<int, 1> in_linesize{CHANNELS_NUM * context_->width};

  // sws_getCachedContext may return the existing context or a new one; manage
  // the raw pointer manually and keep ownership in sws_context_.
  auto *raw_sws = sws_context_.release();
  raw_sws = sws_getCachedContext(raw_sws,
                                 context_->width,
                                 context_->height,
                                 AV_PIX_FMT_YUV420P,
                                 context_->width,
                                 context_->height,
                                 AV_PIX_FMT_RGB32,
                                 0,
                                 nullptr,
                                 nullptr,
                                 nullptr);
  sws_context_.reset(raw_sws);

  auto rgb_frame_data = reinterpret_cast<std::uint8_t *>(rgb_frame_->data());
  sws_scale(sws_context_.get(),
            &frame_->data[0],
            frame_->linesize,
            0,
            context_->height,
            &rgb_frame_data,
            in_linesize.data());
}

void Decoder::fill_async_buffer(const std::byte *data, const std::size_t size) {
  const auto lock = std::lock_guard{async_buffer_mutex_};

  async_buffer_.insert(async_buffer_.end(),
                       reinterpret_cast<const std::uint8_t *>(data),
                       reinterpret_cast<const std::uint8_t *>(data + size));
}

void Decoder::consume_async_buffer() {
  const auto lock = std::lock_guard{async_buffer_mutex_};

  if (async_buffer_.empty()) {
    return;
  }

  const auto size = async_buffer_.size();
  const auto data = async_buffer_.data();
  async_buffer_.clear();

  incoming_data(reinterpret_cast<const std::byte *>(data), size);
}
} // namespace streaming
