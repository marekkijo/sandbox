#pragma once

#include "streaming_common/decoder.hpp"
#include "streaming_common/frame_data.hpp"
#ifdef STREAMING_PIPELINE_STATS
# include "streaming_common/pipeline_stats.hpp"
#endif
#include "streaming_common/video_stream_info.hpp"

#include <gp/gl/buffer_object.hpp>
#include <gp/gl/shader_program.hpp>
#include <gp/gl/texture_object.hpp>
#include <gp/gl/vertex_array_object.hpp>
#include <gp/misc/event.hpp>
#include <gp/sdl/scene_3d.hpp>

#include <array>

#include <functional>
#include <memory>
#include <string>

namespace streaming {
class DecodeScene : public gp::sdl::Scene3D {
public:
  DecodeScene();

  void init(const VideoStreamInfo &video_stream_info);
  void consume_data(const std::byte *data, const std::size_t size, const bool eof = false);

  void set_event_callback(std::function<void(const gp::misc::Event &event)> event_callback);

#ifdef STREAMING_PIPELINE_STATS
  void set_stats_log(std::FILE *out) noexcept;
  void set_max_stats_reports(uint32_t n) noexcept;
#endif

private:
  void init(const int width, const int height, const std::string &title);

  void loop(const gp::misc::Event &event) override;

  void initialize();
  void finalize();
  void decode();
  bool redraw();

  void init_streaming();
  void init_scene();

  VideoStreamInfo video_stream_info_;
  int ms_per_frame_{};
  int frame_counter_{};
  std::unique_ptr<Decoder> decoder_;

  std::shared_ptr<FrameData> rgb_frame_{};
  std::shared_ptr<FrameData> display_frame_{};
  bool frame_ready_{};

  std::unique_ptr<gp::gl::VertexArrayObject> vao_{};
  std::unique_ptr<gp::gl::BufferObject> vertex_buffer_{};
  std::unique_ptr<gp::gl::TextureObject> frame_texture_{};
  std::unique_ptr<gp::gl::ShaderProgram> shader_program_{};

  std::array<std::unique_ptr<gp::gl::BufferObject>, 2> pbo_{};
  int pbo_index_{0};
  bool pbo_primed_{false};

  std::function<void(const gp::misc::Event &event)> event_callback_{};

#ifdef STREAMING_PIPELINE_STATS
  DecodeStats::Frame pending_decode_frame_{};
  DecodeStats decode_stats_{};
#endif
};
} // namespace streaming
