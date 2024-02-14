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
Decoder::Decoder() : frame_{av_frame_alloc()}, packet_{av_packet_alloc()} {
  if (!frame_) { throw std::runtime_error{"av_frame_alloc failed"}; }
  if (!packet_) { throw std::runtime_error{"av_packet_alloc failed"}; }
}

Decoder::~Decoder() {
  fflush(stdout);

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

  auto used = 0;
  if (!try_parse(&used)) { return false; }

  auto success = decode_frame();
  buffer_.erase(buffer_.begin(), buffer_.begin() + used);
  if (success) {
    yuv_to_rgb();
    av_frame_unref(frame_);
  }
  return success;
}

void Decoder::incoming_data(const std::byte *data, const std::size_t size) {
  const auto lg = std::lock_guard{mutex_};
  buffer_.insert(buffer_.end(),
                 reinterpret_cast<const std::uint8_t *>(data),
                 reinterpret_cast<const std::uint8_t *>(data + size));
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

  if (codec_->capabilities & AV_CODEC_CAP_TRUNCATED) { context_->flags |= AV_CODEC_FLAG_TRUNCATED; }

  if (avcodec_open2(context_, codec_, nullptr) < 0) { throw std::runtime_error{"avcodec_open2 failed"}; }

  if (video_stream_info_callback_) { video_stream_info_callback_(video_stream_info); }
}

bool Decoder::try_parse(int *used) {
  if (buffer_.size() == 0u) { return false; }

  std::uint8_t *poutbuf      = nullptr;
  auto          poutbuf_size = 0;

  *used = av_parser_parse2(parser_,
                           context_,
                           &poutbuf,
                           &poutbuf_size,
                           buffer_.data(),
                           buffer_.size(),
                           0,
                           0,
                           AV_NOPTS_VALUE);

  if (*used == 0 || poutbuf_size != *used) { return false; }

  packet_->data = poutbuf;
  packet_->size = poutbuf_size;
  return true;
}

bool Decoder::decode_frame() {
  auto got_picture = 0;
  if (avcodec_decode_video2(context_, frame_, &got_picture, packet_) < 0) { printf("avcodec_decode_video2 failed"); }
  av_packet_unref(packet_);
  return got_picture != 0;
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
