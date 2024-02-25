#include "decoder.hpp"

#include <chrono>
#include <stdexcept>

namespace streaming {
Decoder::~Decoder() { destroy(); }

std::shared_ptr<std::vector<std::uint8_t>> &Decoder::rgb_frame() { return rgb_frame_; }

void Decoder::set_video_stream_info_callback(
    std::function<void(const VideoStreamInfo &video_stream_info)> video_stream_info_callback) {
  video_stream_info_callback_ = video_stream_info_callback;
}

Decoder::Status Decoder::decode() {
  if (!context_) { return {Status::Code::EOS}; }

  if (packet_sent_) {
    auto result = avcodec_receive_frame(context_, frame_);

    if (result == AVERROR(EAGAIN)) {
      packet_sent_ = false;
      return decode();
    }
    if (result == AVERROR_EOF) {
      destroy();
      return {Status::Code::EOS};
    }
    if (result < 0) { return {Status::Code::ERROR}; }

    yuv_to_rgb();
    return {Status::Code::OK, context_->frame_number};
  } else {
    if (upload()) {
      return {Status::Code::RETRY};
    } else {
      return {Status::Code::NODATA};
    }
  }
}

bool Decoder::incoming_data(const std::byte *data, const std::size_t size) {
  const auto lg = std::lock_guard{mutex_};
  if (signaled_eof_) { return false; }

  if (buffer_.empty()) {
    buffer_.reserve(size + NULL_PADDING.size());
  } else {
    buffer_.erase(buffer_.begin() + static_cast<std::int64_t>(buffer_.size()) - NULL_PADDING.size(), buffer_.end());
    buffer_.reserve(buffer_.size() + size + NULL_PADDING.size());
  }

  buffer_.insert(buffer_.end(),
                 reinterpret_cast<const std::uint8_t *>(data),
                 reinterpret_cast<const std::uint8_t *>(data + size));
  buffer_.insert(buffer_.end(), NULL_PADDING.begin(), NULL_PADDING.end());

  return true;
}

void Decoder::signal_eof() {
  const auto lg = std::lock_guard{mutex_};
  signaled_eof_ = true;
}

void Decoder::set_video_stream_info(const VideoStreamInfo &video_stream_info) {
  rgb_frame_ =
      std::make_shared<std::vector<std::uint8_t>>(video_stream_info.width * video_stream_info.height * CHANNELS_NUM);

  packet_ = av_packet_alloc();
  if (packet_ == nullptr) { throw std::runtime_error{"av_packet_alloc failed"}; }

  codec_ = avcodec_find_decoder(video_stream_info.codec_id);
  if (codec_ == nullptr) { throw std::runtime_error{"avcodec_find_decoder failed"}; }

  parser_ = av_parser_init(codec_->id);
  if (parser_ == nullptr) { throw std::runtime_error{"av_parser_init failed"}; }

  context_ = avcodec_alloc_context3(codec_);
  if (context_ == nullptr) { throw std::runtime_error{"avcodec_alloc_context3 failed"}; }
  context_->width = video_stream_info.width;
  context_->height = video_stream_info.height;
  context_->thread_count = DECODE_THREAD_COUNT;

  if (avcodec_open2(context_, codec_, nullptr) < 0) { throw std::runtime_error{"avcodec_open2 failed"}; }

  frame_ = av_frame_alloc();
  if (frame_ == nullptr) { throw std::runtime_error{"av_frame_alloc failed"}; }

  packet_sent_ = false;
  {
    const auto lg = std::lock_guard{mutex_};
    buffer_.clear();
    signaled_eof_ = false;
  }

  if (video_stream_info_callback_) { video_stream_info_callback_(video_stream_info); }
}

bool Decoder::upload() {
  auto ul = std::unique_lock{mutex_};

  const auto eof = buffer_.empty() && signaled_eof_;
  while (!buffer_.empty() || eof) {
    const auto size = buffer_.empty() ? 0u : (buffer_.size() - NULL_PADDING.size());
    auto result = av_parser_parse2(parser_,
                                   context_,
                                   &packet_->data,
                                   &packet_->size,
                                   buffer_.data(),
                                   size,
                                   AV_NOPTS_VALUE,
                                   AV_NOPTS_VALUE,
                                   0);

    auto used = result;

    if (packet_->size != 0) {
      result = avcodec_send_packet(context_, packet_);
      if (result == 0 || result == AVERROR(EAGAIN)) {
        reduce_buffer(used);
        packet_sent_ = true;
        return true;
      } else {
        throw std::runtime_error("avcodec_send_packet data failed");
      }
    } else if (eof) {
      break;
    } else {
      reduce_buffer(used);
      ul.unlock();
      return upload();
    }
  }

  if (signaled_eof_) {
    auto result = avcodec_send_packet(context_, nullptr);
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
  if (buffer_.empty()) { return; }

  if (static_cast<std::size_t>(n) == (buffer_.size() - NULL_PADDING.size())) {
    buffer_.clear();
  } else {
    buffer_.erase(buffer_.begin(), buffer_.begin() + n);
  }
}

void Decoder::destroy() {
  if (context_) {
    av_frame_free(&frame_);
    frame_ = nullptr;

    avcodec_free_context(&context_);
    context_ = nullptr;

    av_parser_close(parser_);
    parser_ = nullptr;

    av_packet_free(&packet_);
    packet_ = nullptr;
  }
}

void Decoder::yuv_to_rgb() {
  const std::array<int, 1> in_linesize{CHANNELS_NUM * context_->width};

  sws_context_ = sws_getCachedContext(sws_context_,
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

  auto *const rgb_frame_data = rgb_frame_->data();
  sws_scale(sws_context_, &frame_->data[0], frame_->linesize, 0, context_->height, &rgb_frame_data, in_linesize.data());
}
} // namespace streaming
