#pragma once

#include "streaming_common/decoder.hpp"
#include "streaming_common/frame_data.hpp"
#include "streaming_common/video_stream_info.hpp"

#include <gp/gl/buffer_object.hpp>
#include <gp/gl/shader_program.hpp>
#include <gp/gl/texture_object.hpp>
#include <gp/gl/vertex_array_object.hpp>
#include <gp/sdl/scene_3d.hpp>

#include <cstdint>
#include <fstream>
#include <memory>
#include <string>

namespace streaming {
class DecodeScene : public gp::sdl::Scene3D {
public:
  DecodeScene(const VideoStreamInfo &video_stream_info);

private:
  void init(const int width, const int height, const std::string &title);

  void loop(const gp::misc::Event &event) override;

  void initialize();
  void finalize();
  void decode();
  bool redraw();

  void init_streaming();
  void init_scene();
  void read_some();

  const VideoStreamInfo video_stream_info_;
  const int ms_per_frame_{};
  int frame_counter_{};
  std::uint64_t last_timestamp_ms_{};
  std::unique_ptr<std::ifstream> input_file_{};
  std::unique_ptr<Decoder> decoder_;

  std::shared_ptr<FrameData> rgb_frame_{};
  bool frame_ready_{};

  std::unique_ptr<gp::gl::VertexArrayObject> vao_{};
  std::unique_ptr<gp::gl::BufferObject> vertex_buffer_{};
  std::unique_ptr<gp::gl::TextureObject> frame_texture_{};
  std::unique_ptr<gp::gl::ShaderProgram> shader_program_{};
};
} // namespace streaming
