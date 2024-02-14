#pragma once

extern "C" {
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
}

#ifdef _WIN32
# include <windows.h>
#endif

#include <GL/gl.h>

#include <fstream>
#include <memory>
#include <string>
#include <vector>

namespace streaming {
class Encoder {
public:
  Encoder(const Encoder &) = delete;
  Encoder &operator=(const Encoder &) = delete;
  Encoder(Encoder &&other) noexcept = delete;
  Encoder &operator=(Encoder &&other) noexcept = delete;

  Encoder(int width,
          int height,
          std::shared_ptr<std::vector<GLubyte>> &gl_frame,
          std::uint16_t fps = 30u,
          AVCodecID codec_id = AV_CODEC_ID_H264);
  ~Encoder();

  void encode_frame();

private:
  std::shared_ptr<std::vector<GLubyte>> gl_frame_{};

  std::vector<std::uint8_t> rgb_frame_{};
  AVFrame *frame_{nullptr};
  AVPacket *packet_{nullptr};
  const AVCodec *codec_{nullptr};
  AVCodecContext *context_{nullptr};

  std::int64_t frame_num_{0};
  std::ofstream file_{};
  SwsContext *sws_context_{nullptr};

  void flip_frame();
  void rgb_to_yuv();
  void close_stream();
};
} // namespace streaming
