#pragma once

#include "common.hpp"
#include "ffmpeg.hpp"
#include "opengl.hpp"

#include <fstream>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace streaming {
class Encoder {
public:
  Encoder() = default;
  Encoder(const Encoder &) = delete;
  Encoder &operator=(const Encoder &) = delete;
  Encoder(Encoder &&other) noexcept = delete;
  Encoder &operator=(Encoder &&other) noexcept = delete;

  ~Encoder();

  void open_stream(const VideoStreamInfo &video_stream_info);
  void close_stream();

  [[nodiscard]] std::shared_ptr<std::vector<GLubyte>> &gl_frame();
  [[nodiscard]] VideoStreamInfo get_video_stream_info() const;
  void set_video_stream_callback(
      std::function<void(const std::byte *data, const std::size_t size, const bool eof)> video_stream_callback);
  void encode();

private:
  void encode_frame(AVFrame *frame);
  void destroy();
  void flip_frame();
  void rgb_to_yuv();

  std::function<void(const std::byte *data, const std::size_t size, const bool eof)> video_stream_callback_{};

  std::shared_ptr<std::vector<GLubyte>> gl_frame_{};
  std::vector<std::uint8_t> rgb_frame_{};

  const AVCodec *codec_{nullptr};
  AVCodecContext *context_{nullptr};
  AVPacket *packet_{nullptr};
  AVFrame *frame_{nullptr};

  SwsContext *sws_context_{nullptr};
};
} // namespace streaming
