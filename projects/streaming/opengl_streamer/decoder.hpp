#pragma once

extern "C" {
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
}

#include <GL/gl.h>

#include <fstream>
#include <string>
#include <vector>

namespace streaming {
class Decoder {
public:
  Decoder(const Decoder &)                     = delete;
  Decoder &operator=(const Decoder &)          = delete;
  Decoder(Decoder &&other) noexcept            = delete;
  Decoder &operator=(Decoder &&other) noexcept = delete;

  Decoder(AVCodecID codec_id = AV_CODEC_ID_H264);
  ~Decoder();

  void get_frame();

private:
  AVFrame              *frame_{nullptr};
  AVPacket             *packet_{nullptr};
  AVCodecParserContext *parser_{nullptr};
  AVCodec              *codec_{nullptr};
  AVCodecContext       *context_{nullptr};

  std::int64_t              frame_num_{0};
  std::ifstream             file_{};
  std::vector<std::uint8_t> buffer_{};
  SwsContext               *sws_context_{nullptr};

  [[nodiscard]] bool try_parse(int *used);
  [[nodiscard]] bool read_more();
  [[nodiscard]] bool decode_frame();
};
} // namespace streaming
