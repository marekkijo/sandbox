#pragma once

#include "common/common.hpp"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
}

#ifdef _WIN32
# include <windows.h>
#endif

#include <GL/gl.h>

#include <fstream>
#include <functional>
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

  Encoder(int width, int height, std::uint16_t fps, AVCodecID codec_id = AV_CODEC_ID_H264);
  ~Encoder();

  [[nodiscard]] std::shared_ptr<std::vector<GLubyte>> &gl_frame();
  [[nodiscard]] VideoStreamInfo get_video_stream_info() const;
  void
  set_video_stream_callback(std::function<void(const std::byte *data, const std::size_t size)> video_stream_callback);
  void encode_frame();
  void close_stream();

private:
  std::shared_ptr<std::vector<GLubyte>> gl_frame_{};

  std::function<void(const std::byte *data, const std::size_t size)> video_stream_callback_{};

  std::vector<std::uint8_t> rgb_frame_{};
  AVFrame *frame_{nullptr};
  AVPacket *packet_{nullptr};
  const AVCodec *codec_{nullptr};
  AVCodecContext *context_{nullptr};

  std::int64_t frame_num_{0};
  SwsContext *sws_context_{nullptr};

  void flip_frame();
  void rgb_to_yuv();
};
} // namespace streaming
