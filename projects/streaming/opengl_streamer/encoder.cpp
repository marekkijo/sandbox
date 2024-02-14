#include "encoder.hpp"

extern "C" {
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
}

#include <stdexcept>

namespace {
constexpr auto CHANNELS_NUM = std::size_t{4u};
} // namespace

namespace streaming {
Encoder::Encoder(int width,
                 int height,
                 std::shared_ptr<std::vector<GLubyte>> &gl_frame,
                 std::uint16_t fps,
                 AVCodecID codec_id)
    : gl_frame_{gl_frame}
    , rgb_frame_(gl_frame_->size())
    , frame_{av_frame_alloc()}
    , packet_{av_packet_alloc()}
    , codec_{avcodec_find_encoder(codec_id)}
    , context_{codec_ ? avcodec_alloc_context3(codec_) : nullptr} {
  if (!frame_) { throw std::runtime_error{"av_frame_alloc failed"}; }
  if (!packet_) { throw std::runtime_error{"av_packet_alloc failed"}; }
  if (!codec_) { throw std::runtime_error{"avcodec_find_encoder failed"}; }
  if (!context_) { throw std::runtime_error{"avcodec_alloc_context3 failed"}; }

  const auto pixel_format = AV_PIX_FMT_YUV420P;

  context_->bit_rate = 131072 * 2;
  context_->width = width;
  context_->height = height;
  context_->time_base.num = 1;
  context_->time_base.den = fps;
  context_->gop_size = fps / 3;
  context_->max_b_frames = 1;
  context_->pix_fmt = pixel_format;

  if (codec_id == AV_CODEC_ID_H264) { av_opt_set(context_->priv_data, "preset", "slow", 0); }

  if (avcodec_open2(context_, codec_, nullptr) < 0) { throw std::runtime_error{"avcodec_open2 failed"}; }

  frame_->format = pixel_format;
  frame_->width = width;
  frame_->height = height;

  if (av_image_alloc(frame_->data, frame_->linesize, width, height, pixel_format, 32) < 0) {
    throw std::runtime_error{"av_image_alloc failed"};
  }

  file_.open("file.h264", std::ios::binary);
  if (!file_.is_open()) { throw std::runtime_error{"cannot open file"}; }
}

Encoder::~Encoder() {
  close_stream();

  av_freep(&frame_->data[0]);
  avcodec_free_context(&context_);
  av_packet_free(&packet_);
  av_frame_free(&frame_);
}

void Encoder::encode_frame() {
  printf("encoding frame %li\n", frame_num_);

  flip_frame();
  rgb_to_yuv();

  frame_->pts = frame_num_++;

  int got_output;
  if (avcodec_encode_video2(context_, packet_, frame_, &got_output) < 0) {
    throw std::runtime_error{"avcodec_encode_video2 failed"};
  }
  if (got_output) {
    file_.write(reinterpret_cast<char *>(packet_->data), packet_->size);
    av_packet_unref(packet_);
  }
}

void Encoder::flip_frame() {
  const auto width = context_->width;
  const auto height = context_->height;

  for (std::size_t i = 0; i < height; i++) {
    for (std::size_t j = 0; j < width; j++) {
      auto ptr_gl = gl_frame_->data() + (CHANNELS_NUM * (width * (height - i - 1) + j));
      auto ptr_rgb = rgb_frame_.data() + (CHANNELS_NUM * (width * i + j));
      memcpy(ptr_rgb, ptr_gl, CHANNELS_NUM);
    }
  }
}

void Encoder::rgb_to_yuv() {
  const int in_linesize[1] = {CHANNELS_NUM * context_->width};

  sws_context_ = sws_getCachedContext(sws_context_,
                                      context_->width,
                                      context_->height,
                                      AV_PIX_FMT_RGB32,
                                      context_->width,
                                      context_->height,
                                      AV_PIX_FMT_YUV420P,
                                      0,
                                      NULL,
                                      NULL,
                                      NULL);

  const auto rgb_frame_data = rgb_frame_.data();
  sws_scale(sws_context_, &rgb_frame_data, in_linesize, 0, context_->height, frame_->data, frame_->linesize);
}

void Encoder::close_stream() {
  int got_output;
  do {
    fflush(stdout);
    if (avcodec_encode_video2(context_, packet_, NULL, &got_output) < 0) {
      throw std::runtime_error{"avcodec_encode_video2 failed"};
    }
    if (got_output) {
      file_.write(reinterpret_cast<char *>(packet_->data), packet_->size);
      av_packet_unref(packet_);
    }
  } while (got_output);

  uint8_t endcode[] = {0, 0, 1, 0xb7};
  file_.write(reinterpret_cast<char *>(endcode), sizeof(endcode));
}
} // namespace streaming
