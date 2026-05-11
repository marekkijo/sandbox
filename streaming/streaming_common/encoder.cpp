#include "encoder.hpp"

#include "streaming_common/constants.hpp"

#include <array>
#include <chrono>
#include <stdexcept>
#include <string>

namespace streaming {
Encoder::Encoder(const VideoStreamInfo &video_stream_info)
    : video_frame_{std::make_shared<FrameData>(video_stream_info.width * video_stream_info.height * CHANNELS_NUM)}
    , codec_{avcodec_find_encoder(video_stream_info.codec_id)}
    , context_{codec_ ? avcodec_alloc_context3(codec_) : nullptr}
    , packet_{av_packet_alloc()} {
  if (codec_ == nullptr) {
    throw std::runtime_error{"avcodec_find_encoder failed"};
  }
  if (!context_) {
    throw std::runtime_error{"avcodec_alloc_context3 failed"};
  }
  if (!packet_) {
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

  if (avcodec_open2(context_.get(), codec_, nullptr) < 0) {
    throw std::runtime_error{"avcodec_open2 failed"};
  }

  frame_.reset(av_frame_alloc());
  if (!frame_) {
    throw std::runtime_error{"av_frame_alloc failed"};
  }

  frame_->format = context_->pix_fmt;
  frame_->width = context_->width;
  frame_->height = context_->height;
  frame_->pts = 0;

  if (av_frame_get_buffer(frame_.get(), 0) < 0) {
    throw std::runtime_error{"av_frame_get_buffer failed"};
  }

  sws_context_.reset(sws_getContext(context_->width,
                                    context_->height,
                                    AV_PIX_FMT_RGBA,
                                    context_->width,
                                    context_->height,
                                    AV_PIX_FMT_YUV420P,
                                    SWS_FAST_BILINEAR,
                                    nullptr,
                                    nullptr,
                                    nullptr));
  if (!sws_context_) {
    throw std::runtime_error{"sws_getContext failed"};
  }
}

Encoder::~Encoder() {
  if (context_) {
    try {
      encode_frame(nullptr);
    } catch (...) {}
  }
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
  if (av_frame_make_writable(frame_.get()) < 0) {
    throw std::runtime_error{"av_frame_make_writable failed"};
  }

#ifdef STREAMING_PIPELINE_STATS
  using Clock = std::chrono::steady_clock;
  const auto t0 = Clock::now();
#endif
  rgb_to_yuv();
#ifdef STREAMING_PIPELINE_STATS
  const auto t1 = Clock::now();
#endif
  encode_frame(frame_.get());
#ifdef STREAMING_PIPELINE_STATS
  const auto t2 = Clock::now();

  last_timings_.rgb_to_yuv_us = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0);
  last_timings_.encode_us = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
#endif

  frame_->pts++;
}

void Encoder::encode_frame(AVFrame *frame) {
  for (;;) {
    const auto send_result = avcodec_send_frame(context_.get(), frame);
    if (send_result == AVERROR_EOF) {
      return;
    }
    if (send_result < 0 && send_result != AVERROR(EAGAIN)) {
      char errbuf[AV_ERROR_MAX_STRING_SIZE]{};
      av_strerror(send_result, errbuf, sizeof(errbuf));
      throw std::runtime_error{std::string{"avcodec_send_frame failed: "} + errbuf};
    }

    bool drained = false;
    while (!drained) {
      const auto rc = avcodec_receive_packet(context_.get(), packet_.get());
      if (rc == AVERROR(EAGAIN)) {
        drained = true;
      } else if (rc == AVERROR_EOF) {
        if (video_stream_callback_) {
          static constexpr std::array<uint8_t, 4> endcode{0, 0, 1, 0xb7};
          video_stream_callback_(reinterpret_cast<const std::byte *>(endcode.data()), sizeof(endcode), true);
        }
        av_packet_unref(packet_.get());
        return;
      } else if (rc < 0) {
        char errbuf[AV_ERROR_MAX_STRING_SIZE]{};
        av_strerror(rc, errbuf, sizeof(errbuf));
        throw std::runtime_error{std::string{"avcodec_receive_packet failed: "} + errbuf};
      } else {
        if (video_stream_callback_) {
          video_stream_callback_(reinterpret_cast<const std::byte *>(packet_->data),
                                 static_cast<std::size_t>(packet_->size),
                                 false);
        }
        av_packet_unref(packet_.get());
      }
    }

    if (send_result == 0) {
      return;
    }
    // send_result == AVERROR(EAGAIN): drained, retry send
  }
}

void Encoder::rgb_to_yuv() {
  const auto width = static_cast<std::size_t>(context_->width);
  const auto height = static_cast<std::size_t>(context_->height);

  // GL framebuffers are stored bottom-up: video_frame_->data() holds the bottom
  // screen row, and the last row in memory is the top screen row. By setting
  // src[0] to that last (top-screen) row and using a negative linesize, each
  // successive sws_scale input row steps backward in memory — effectively reading
  // the GL buffer top-to-bottom in screen coordinates, which is what the encoder
  // expects. No intermediate copy is needed.
  const auto *last_row =
      reinterpret_cast<const std::uint8_t *>(video_frame_->data()) + (height - 1) * width * CHANNELS_NUM;
  const std::uint8_t *src[] = {last_row};
  const std::array<int, 1> in_linesize{-static_cast<int>(CHANNELS_NUM * width)};
  sws_scale(sws_context_.get(), src, in_linesize.data(), 0, context_->height, frame_->data, frame_->linesize);
}

} // namespace streaming
