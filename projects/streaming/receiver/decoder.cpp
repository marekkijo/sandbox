#include "decoder.hpp"

extern "C" {
#include <libavutil/avutil.h>
#include <libavutil/opt.h>
}

#include <chrono>
#include <stdexcept>

namespace {
constexpr auto CHANNELS_NUM = std::size_t{4u};
} // namespace

namespace streaming {
Decoder::Decoder()
    : frame_{av_frame_alloc()}
    , packet_{av_packet_alloc()} {
  if (!frame_) { throw std::runtime_error{"av_frame_alloc failed"}; }
  if (!packet_) { throw std::runtime_error{"av_packet_alloc failed"}; }
}

Decoder::~Decoder() {
  avcodec_free_context(&context_);
  av_parser_close(parser_);
  av_packet_free(&packet_);
  av_frame_free(&frame_);
}

std::shared_ptr<std::vector<std::uint8_t>> &Decoder::rgb_frame() { return rgb_frame_; }

void Decoder::set_video_stream_info_callback(
    std::function<void(const VideoStreamInfo &video_stream_info)> video_stream_info_callback) {
  video_stream_info_callback_ = video_stream_info_callback;
}

bool Decoder::prepare_frame() {
  const auto lg = std::lock_guard{mutex_};

  auto result = avcodec_receive_frame(context_, frame_);
  if (!buffer_.empty()) { // upload another packet for decoding
    int used;
    if (upload_package(buffer_.data(), buffer_.size(), &used)) {
      buffer_.erase(buffer_.begin(), buffer_.begin() + used);
    }
  }
  if (result == AVERROR(EAGAIN) || result == AVERROR_EOF || result < 0) { return false; }

  yuv_to_rgb();

  return true;
}

void Decoder::incoming_data(const std::byte *data, const std::size_t size) {
  const auto lg = std::lock_guard{mutex_};

  int used;
  if (buffer_.empty()) {
    if (upload_package(data, size, &used)) {
      if (used != size) { store_on_buffer(data + used, size - used); }
    } else {
      store_on_buffer(data, size);
    }
  } else {
    store_on_buffer(data, size);
    if (upload_package(buffer_.data(), buffer_.size(), &used)) {
      buffer_.erase(buffer_.begin(), buffer_.begin() + used);
    }
  }
}

void Decoder::set_video_stream_info(const VideoStreamInfo &video_stream_info) {
  rgb_frame_ =
      std::make_shared<std::vector<std::uint8_t>>(video_stream_info.width * video_stream_info.height * CHANNELS_NUM);
  parser_ = av_parser_init(video_stream_info.codec_id);
  if (!parser_) { throw std::runtime_error{"av_parser_init failed"}; }
  codec_ = avcodec_find_decoder(video_stream_info.codec_id);
  if (!codec_) { throw std::runtime_error{"avcodec_find_decoder failed"}; }
  context_ = avcodec_alloc_context3(codec_);
  if (!context_) { throw std::runtime_error{"avcodec_alloc_context3 failed"}; }
  context_->width = video_stream_info.width;
  context_->height = video_stream_info.height;
  context_->thread_count = 4;

  if (codec_->capabilities & AV_CODEC_CAP_TRUNCATED) { context_->flags |= AV_CODEC_FLAG_TRUNCATED; }
  if (avcodec_open2(context_, codec_, nullptr) < 0) { throw std::runtime_error{"avcodec_open2 failed"}; }

  if (video_stream_info_callback_) { video_stream_info_callback_(video_stream_info); }

  avcodec_flush_buffers(context_);
}

bool Decoder::upload_package(const void *data, const std::size_t size, int *used) {
  auto result = av_parser_parse2(parser_,
                                 context_,
                                 &packet_->data,
                                 &packet_->size,
                                 reinterpret_cast<const std::uint8_t *>(data),
                                 size,
                                 AV_NOPTS_VALUE,
                                 AV_NOPTS_VALUE,
                                 0);
  if (result < 0) { return false; }
  *used = result;
  if (packet_->size != 0) {
    result = avcodec_send_packet(context_, packet_);
    if (result < 0) { return false; }
  }
  return true;
}

void Decoder::store_on_buffer(const void *data, const std::size_t size) {
  buffer_.insert(buffer_.end(),
                 reinterpret_cast<const std::uint8_t *>(data),
                 reinterpret_cast<const std::uint8_t *>(data + size));
}

void Decoder::yuv_to_rgb() {
  const int in_linesize[1] = {CHANNELS_NUM * context_->width};

  sws_context_ = sws_getCachedContext(sws_context_,
                                      context_->width,
                                      context_->height,
                                      AV_PIX_FMT_YUV420P,
                                      context_->width,
                                      context_->height,
                                      AV_PIX_FMT_RGB32,
                                      0,
                                      NULL,
                                      NULL,
                                      NULL);

  const auto rgb_frame_data = rgb_frame_->data();
  sws_scale(sws_context_, &frame_->data[0], frame_->linesize, 0, context_->height, &rgb_frame_data, in_linesize);
}
} // namespace streaming
