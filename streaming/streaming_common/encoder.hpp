#pragma once

#include "streaming_common/frame_data.hpp"
#include "streaming_common/video_stream_info.hpp"

#include <gp/ffmpeg/ffmpeg.hpp>

#include <functional>
#include <memory>

namespace streaming {
class Encoder {
public:
  explicit Encoder(const VideoStreamInfo &video_stream_info);
  Encoder(const Encoder &) = delete;
  Encoder &operator=(const Encoder &) = delete;
  Encoder(Encoder &&other) noexcept = delete;
  Encoder &operator=(Encoder &&other) noexcept = delete;

  ~Encoder();

  std::shared_ptr<FrameData> video_frame();
  VideoStreamInfo video_stream_info() const;
  void set_video_stream_callback(
      std::function<void(const std::byte *data, const std::size_t size, const bool eof)> video_stream_callback);
  void encode();

private:
  void encode_frame(AVFrame *frame);
  void flip_frame();
  void rgb_to_yuv();

  std::function<void(const std::byte *data, const std::size_t size, const bool eof)> video_stream_callback_{};

  std::shared_ptr<FrameData> video_frame_{};
  FrameData rgb_frame_{};

  const AVCodec *codec_{};
  gp::ffmpeg::UniqueAVCodecContext context_{};
  gp::ffmpeg::UniqueAVPacket packet_{};
  gp::ffmpeg::UniqueAVFrame frame_{};

  gp::ffmpeg::UniqueSwsContext sws_context_{};
};
} // namespace streaming
