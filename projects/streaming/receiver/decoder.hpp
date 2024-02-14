#pragma once

#include "common/common.hpp"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
}

#include <GL/gl.h>

#include <functional>
#include <memory>
#include <mutex>
#include <vector>

namespace streaming {
class Decoder {
public:
  Decoder(const Decoder &)                     = delete;
  Decoder &operator=(const Decoder &)          = delete;
  Decoder(Decoder &&other) noexcept            = delete;
  Decoder &operator=(Decoder &&other) noexcept = delete;

  Decoder();
  ~Decoder();

  std::shared_ptr<std::vector<std::uint8_t>> &rgb_frame();
  void                                        set_video_stream_info_callback(
                                             std::function<void(const VideoStreamInfo &video_stream_info)> video_stream_info_callback);
  [[nodiscard]] bool prepare_frame();
  void               incoming_data(const std::byte *data, const std::size_t size);
  void               set_video_stream_info(const VideoStreamInfo &video_stream_info);

private:
  std::shared_ptr<std::vector<std::uint8_t>> rgb_frame_{};

  std::function<void(const VideoStreamInfo &video_stream_info)> video_stream_info_callback_{};

  AVFrame              *frame_{nullptr};
  AVPacket             *packet_{nullptr};
  AVCodecParserContext *parser_{nullptr};
  AVCodec              *codec_{nullptr};
  AVCodecContext       *context_{nullptr};

  std::vector<std::uint8_t> buffer_{};
  SwsContext               *sws_context_{nullptr};

  std::mutex mutex_{};

  [[nodiscard]] bool try_parse(int *used);
  [[nodiscard]] bool decode_frame();

  void yuv_to_rgb();
};
} // namespace streaming
