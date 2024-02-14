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
Decoder::Decoder(int width, int height, AVCodecID codec_id)
    : rgb_frame_{std::make_shared<std::vector<std::uint8_t>>(width * height * CHANNELS_NUM)}
    , frame_{av_frame_alloc()}
    , packet_{av_packet_alloc()}
    , parser_{av_parser_init(codec_id)}
    , codec_{avcodec_find_decoder(codec_id)}
    , context_{codec_ ? avcodec_alloc_context3(codec_) : nullptr} {
  if (!frame_) { throw std::runtime_error{"av_frame_alloc failed"}; }
  if (!packet_) { throw std::runtime_error{"av_packet_alloc failed"}; }
  if (!parser_) { throw std::runtime_error{"av_parser_init failed"}; }
  if (!codec_) { throw std::runtime_error{"avcodec_find_decoder failed"}; }
  if (!context_) { throw std::runtime_error{"avcodec_alloc_context3 failed"}; }

  if (codec_->capabilities & AV_CODEC_CAP_TRUNCATED) { context_->flags |= AV_CODEC_FLAG_TRUNCATED; }

  if (avcodec_open2(context_, codec_, nullptr) < 0) { throw std::runtime_error{"avcodec_open2 failed"}; }

  file_.open("file.h264", std::ios::binary);
  if (!file_.is_open()) { throw std::runtime_error{"cannot open file"}; }
}

Decoder::~Decoder() {
  fflush(stdout);

  avcodec_free_context(&context_);
  av_parser_close(parser_);
  av_packet_free(&packet_);
  av_frame_free(&frame_);
}

std::shared_ptr<std::vector<std::uint8_t>> &Decoder::rgb_frame() { return rgb_frame_; }

bool Decoder::prepare_frame() {
  auto used = 0;
  do {
    if (try_parse(&used)) { break; }
    if (!read_more()) {
      std::fill(rgb_frame_->begin(), rgb_frame_->end(), 0u);
      printf("decoded frame %li\n", frame_num_++);
      return true;
    }
  } while (true);

  auto success = decode_frame();
  buffer_.erase(buffer_.begin(), buffer_.begin() + used);
  if (success) {
    yuv_to_rgb();
    printf("decoded frame %li\n", frame_num_++);
    av_frame_unref(frame_);
  }
  return success;
}

bool Decoder::try_parse(int *used) {
  if (buffer_.size() == 0) { return false; }

  std::uint8_t *poutbuf = nullptr;
  auto poutbuf_size = 0;

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

bool Decoder::read_more() {
  if (file_.eof()) { return false; }

  const std::size_t read_size = 16384u;
  const auto current_size = buffer_.size();
  buffer_.resize(current_size + read_size);
  file_.read(reinterpret_cast<char *>(buffer_.data() + current_size), read_size);
  buffer_.resize(current_size + file_.gcount());
  return true;
}

bool Decoder::decode_frame() {
  auto got_picture = 0;
  if (avcodec_decode_video2(context_, frame_, &got_picture, packet_) < 0) {
    throw std::runtime_error{"avcodec_decode_video2 failed"};
  }
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
