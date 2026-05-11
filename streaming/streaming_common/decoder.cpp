#include "decoder.hpp"

#include "streaming_common/constants.hpp"

#include <libyuv.h>

#include <chrono>
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
  // Single-threaded decoding: frame-threading buffers N frames in flight which
  // adds N×frame_time of fixed latency — unacceptable for interactive streaming.
  context_->thread_count = 1;
  // Output frames immediately without reordering; safe because the encoder
  // uses max_b_frames=0 so there are no B-frames to reorder.
  context_->flags |= AV_CODEC_FLAG_LOW_DELAY;

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
    buffer_read_offset_ = 0;
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
#ifdef STREAMING_PIPELINE_STATS
    using Clock = std::chrono::steady_clock;
    const auto t0 = Clock::now();
#endif
    auto result = avcodec_receive_frame(context_.get(), frame_.get());
#ifdef STREAMING_PIPELINE_STATS
    const auto t1 = Clock::now();
    last_timings_.receive_us = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0);
#endif

    if (result == AVERROR(EAGAIN)) {
      packet_sent_ = false;
      return decode();
    }
    if (result == AVERROR_EOF) {
      context_.reset();
      parser_.reset();
      packet_.reset();
      frame_.reset();
      return {Status::Code::EOS};
    }
    if (result < 0) {
      return {Status::Code::ERROR};
    }

    yuv_to_rgb();
#ifdef STREAMING_PIPELINE_STATS
    const auto t2 = Clock::now();
    last_timings_.yuv_to_rgb_us = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
    // upload_us is only valid for the first frame decoded from each uploaded packet;
    // clear it so subsequent frames from the same packet don't inherit a stale value.
    if (!upload_timing_fresh_) {
      last_timings_.upload_us = std::chrono::microseconds::zero();
    }
    upload_timing_fresh_ = false;
#endif
    return {Status::Code::OK, static_cast<int>(context_->frame_num)};
  } else {
#ifdef STREAMING_PIPELINE_STATS
    using Clock = std::chrono::steady_clock;
    const auto t0 = Clock::now();
#endif
    const bool uploaded = upload();
#ifdef STREAMING_PIPELINE_STATS
    const auto t1 = Clock::now();
    if (uploaded) {
      last_timings_.upload_us = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0);
      upload_timing_fresh_ = true;
      return {Status::Code::RETRY};
    }
#else
    if (uploaded) {
      return {Status::Code::RETRY};
    }
#endif
    return {Status::Code::NODATA};
  }
}

bool Decoder::incoming_data(const std::byte *data, const std::size_t size, const bool async) {
  if (!async && !rgb_frame_) {
    throw std::runtime_error{"Decoder is not initialized"};
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
    if (buffer_read_offset_ > 0) {
      buffer_.erase(buffer_.begin(), buffer_.begin() + static_cast<std::ptrdiff_t>(buffer_read_offset_));
      buffer_read_offset_ = 0;
    }
    if (buffer_.size() >= NULL_PADDING.size()) {
      buffer_.erase(buffer_.end() - NULL_PADDING.size(), buffer_.end());
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
  for (;;) {
    consume_async_buffer();
    const auto eof = buffer_.empty() && signaled_eof_;
    if (buffer_.empty() && !eof) {
      break;
    }

    const auto size = buffer_.empty() ? 0u : (buffer_.size() - buffer_read_offset_ - NULL_PADDING.size());
    auto result = av_parser_parse2(parser_.get(),
                                   context_.get(),
                                   &packet_->data,
                                   &packet_->size,
                                   buffer_.data() + buffer_read_offset_,
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
    } else if (used == 0) {
      // Parser consumed nothing and produced no packet — need more input data.
      break;
    } else {
      reduce_buffer(used);
      // loop back to consume any new async data and retry parsing
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

void Decoder::reduce_buffer(int n) {
  if (buffer_.empty()) {
    return;
  }

  buffer_read_offset_ += static_cast<std::size_t>(n);
  if (buffer_read_offset_ >= buffer_.size() - NULL_PADDING.size()) {
    buffer_.clear();
    buffer_read_offset_ = 0;
  }
}

void Decoder::yuv_to_rgb() {
  // YUV420P (I420) → RGBA (= libyuv ABGR): fast SIMD path via libyuv.
  auto *dst = reinterpret_cast<std::uint8_t *>(rgb_frame_->data());
  const int dst_stride = context_->width * CHANNELS_NUM;
  libyuv::I420ToABGR(frame_->data[0],
                     frame_->linesize[0],
                     frame_->data[1],
                     frame_->linesize[1],
                     frame_->data[2],
                     frame_->linesize[2],
                     dst,
                     dst_stride,
                     context_->width,
                     context_->height);
}

void Decoder::fill_async_buffer(const std::byte *data, const std::size_t size) {
  const auto lock = std::lock_guard{async_buffer_mutex_};

  async_buffer_.insert(async_buffer_.end(),
                       reinterpret_cast<const std::uint8_t *>(data),
                       reinterpret_cast<const std::uint8_t *>(data + size));
}

void Decoder::consume_async_buffer() {
  std::vector<std::uint8_t> local;
  {
    const auto lock = std::lock_guard{async_buffer_mutex_};
    if (async_buffer_.empty()) {
      return;
    }
    local.swap(async_buffer_);
  }
  incoming_data(reinterpret_cast<const std::byte *>(local.data()), local.size());
}
} // namespace streaming
