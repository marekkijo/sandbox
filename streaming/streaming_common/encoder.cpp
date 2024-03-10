#include "encoder.hpp"

#include "streaming_common/constants.hpp"

#include <array>
#include <stdexcept>

namespace streaming {
Encoder::Encoder(const VideoStreamInfo &video_stream_info)
    : video_frame_{std::make_shared<FrameData>(video_stream_info.width * video_stream_info.height * CHANNELS_NUM)}
    , rgb_frame_(video_frame_->size())
    , codec_{avcodec_find_encoder(video_stream_info.codec_id)}
    , context_{codec_ ? avcodec_alloc_context3(codec_) : nullptr}
    , packet_{av_packet_alloc()} {
  if (codec_ == nullptr) {
    destroy();
    throw std::runtime_error{"avcodec_find_encoder failed"};
  }
  if (context_ == nullptr) {
    destroy();
    throw std::runtime_error{"avcodec_alloc_context3 failed"};
  }
  if (packet_ == nullptr) {
    destroy();
    throw std::runtime_error{"av_packet_alloc failed"};
  }

  context_->bit_rate = ENCODE_BITRATE;
  context_->width = video_stream_info.width;
  context_->height = video_stream_info.height;
  context_->time_base = {1, video_stream_info.fps};
  context_->framerate = {video_stream_info.fps, 1};
  context_->gop_size = video_stream_info.fps;
  context_->max_b_frames = 0;
  context_->pix_fmt = AV_PIX_FMT_YUV420P;
  context_->thread_count = ENCODE_THREAD_COUNT;

  if (codec_->id == AV_CODEC_ID_H264) {
    av_opt_set(context_->priv_data, "preset", "ultrafast", 0);
    av_opt_set(context_->priv_data, "tune", "zerolatency", 0);
  }

  if (avcodec_open2(context_, codec_, nullptr) < 0) {
    destroy();
    throw std::runtime_error{"avcodec_open2 failed"};
  }

  frame_ = av_frame_alloc();
  if (frame_ == nullptr) {
    destroy();
    throw std::runtime_error{"av_frame_alloc failed"};
  }

  frame_->format = context_->pix_fmt;
  frame_->width = context_->width;
  frame_->height = context_->height;
  frame_->pts = 0;

  if (av_frame_get_buffer(frame_, 0) < 0) {
    destroy();
    throw std::runtime_error{"av_frame_get_buffer failed"};
  }

  sws_context_ = sws_getContext(context_->width,
                                context_->height,
                                AV_PIX_FMT_RGB32,
                                context_->width,
                                context_->height,
                                AV_PIX_FMT_YUV420P,
                                SWS_BILINEAR,
                                nullptr,
                                nullptr,
                                nullptr);
  if (sws_context_ == nullptr) {
    destroy();
    throw std::runtime_error{"sws_getContext failed"};
  }
}

Encoder::~Encoder() {
  if (context_) { encode_frame(nullptr); }

  destroy();
}

std::shared_ptr<FrameData> Encoder::video_frame() { return video_frame_; }

VideoStreamInfo Encoder::video_stream_info() const {
  return {context_->width,
          context_->height,
          static_cast<std::uint16_t>(context_->time_base.den),
          codec_->id,
          std::string{avcodec_get_name(codec_->id)}};
}

void Encoder::set_video_stream_callback(
    std::function<void(const std::byte *data, const std::size_t size, const bool eof)> video_stream_callback) {
  video_stream_callback_ = video_stream_callback;
}

void Encoder::encode() {
  if (av_frame_make_writable(frame_) < 0) { throw std::runtime_error{"av_frame_make_writable failed"}; }

  flip_frame();
  rgb_to_yuv();

  encode_frame(frame_);

  frame_->pts++;
}

void Encoder::encode_frame(AVFrame *frame) {
  auto result = avcodec_send_frame(context_, frame);
  if (result < 0) { throw std::runtime_error{"avcodec_send_frame failed"}; }

  while (result == 0) {
    result = avcodec_receive_packet(context_, packet_);
    if (result == 0) {
      if (video_stream_callback_) {
        video_stream_callback_(reinterpret_cast<const std::byte *>(packet_->data),
                               static_cast<std::size_t>(packet_->size),
                               false);
      }
      av_packet_unref(packet_);
    } else if (result == AVERROR(EAGAIN)) {
      continue;
    } else if (result == AVERROR_EOF) {
      if (video_stream_callback_) {
        static constexpr std::array<uint8_t, 4> endcode{0, 0, 1, 0xb7};
        video_stream_callback_(reinterpret_cast<const std::byte *>(endcode.data()), sizeof(endcode), true);
      }
      av_packet_unref(packet_);
    } else {
      throw std::runtime_error{"avcodec_receive_packet failed: Unknown error"};
    }
  }
}

void Encoder::flip_frame() {
  const auto width = static_cast<std::size_t>(context_->width);
  const auto height = static_cast<std::size_t>(context_->height);

  for (std::size_t i = 0; i < height; i++) {
    for (std::size_t j = 0; j < width; j++) {
      auto *ptr_gl = video_frame_->data() + (CHANNELS_NUM * (width * (height - i - 1) + j));
      auto *ptr_rgb = rgb_frame_.data() + (CHANNELS_NUM * (width * i + j));
      memcpy(ptr_rgb, ptr_gl, CHANNELS_NUM);
    }
  }
}

void Encoder::rgb_to_yuv() {
  const std::array<int, 1> in_linesize{CHANNELS_NUM * context_->width};

  auto rgb_frame_data = reinterpret_cast<std::uint8_t *>(rgb_frame_.data());
  sws_scale(sws_context_, &rgb_frame_data, in_linesize.data(), 0, context_->height, frame_->data, frame_->linesize);
}

void Encoder::destroy() {
  if (sws_context_) {
    sws_freeContext(sws_context_);
    sws_context_ = nullptr;
  }
  if (frame_) {
    av_frame_free(&frame_);
    frame_ = nullptr;
  }
  if (packet_) {
    av_packet_free(&packet_);
    packet_ = nullptr;
  }
  if (context_) {
    avcodec_free_context(&context_);
    context_ = nullptr;
  }
  codec_ = nullptr;
}
} // namespace streaming
