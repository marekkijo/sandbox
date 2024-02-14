#include "decoder.hpp"

extern "C" {
#include <libavutil/avutil.h>
#include <libavutil/opt.h>
}

#include <stdexcept>

namespace streaming {
Decoder::Decoder(AVCodecID codec_id)
    : frame_{av_frame_alloc()}
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

void Decoder::get_frame() {
  auto used = 0;
  do {
    if (try_parse(&used)) { break; }
    if (!read_more()) { return; }
  } while (true);

  auto success = decode_frame();
  buffer_.erase(buffer_.begin(), buffer_.begin() + used);

  if (success) {
    printf("decoded frame %li\n", frame_num_++);
    av_frame_unref(frame_);
  }
}

bool Decoder::try_parse(int *used) {
  if (buffer_.size() == 0) { return false; }

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

  if (poutbuf_size == 0 && *used >= 0) { return false; }

  packet_->data = poutbuf;
  packet_->size = poutbuf_size;
  return true;
}

bool Decoder::read_more() {
  if (file_.eof()) { return false; }

  const std::size_t read_size    = 16384u;
  const auto        current_size = buffer_.size();
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

} // namespace streaming
