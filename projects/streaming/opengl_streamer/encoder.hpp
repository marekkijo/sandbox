#pragma once

extern "C" {
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
}

#include <GL/gl.h>

#include <fstream>
#include <memory>
#include <string>
#include <vector>

namespace streaming {
class Encoder {
public:
  Encoder(const Encoder &)                     = delete;
  Encoder &operator=(const Encoder &)          = delete;
  Encoder(Encoder &&other) noexcept            = delete;
  Encoder &operator=(Encoder &&other) noexcept = delete;

  Encoder(int                                    width,
          int                                    height,
          std::shared_ptr<std::vector<GLubyte>> &gl_frame,
          unsigned int                           fps      = 30u,
          AVCodecID                              codec_id = AV_CODEC_ID_H264);
  ~Encoder();

  void encode_frame();

private:
  int                                   width_{};
  int                                   height_{};
  std::shared_ptr<std::vector<GLubyte>> gl_frame_{};

  std::vector<std::uint8_t> rgb_frame_{};
  int                       channels_{};
  AVPacket                 *packet_{nullptr};
  AVFrame                  *frame_{nullptr};
  AVCodec                  *codec_{nullptr};
  AVCodecContext           *context_{nullptr};

  std::int64_t  frame_num_{0};
  std::ofstream file_{};
  SwsContext   *sws_context_{nullptr};

  void flip_frame();
  void rgb_to_yuv();
  void close_stream();
};
} // namespace streaming
