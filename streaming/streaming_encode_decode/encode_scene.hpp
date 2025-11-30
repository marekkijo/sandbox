#pragma once

#include "streaming_common/encoder.hpp"
#include "streaming_common/frame_data.hpp"
#include "streaming_common/video_stream_info.hpp"

#include <gp/gl/buffer_object.hpp>
#include <gp/gl/shader_program.hpp>
#include <gp/gl/vertex_array_object.hpp>
#include <gp/sdl/scene_3d.hpp>

#include <glm/glm.hpp>

#include <cstdint>
#include <fstream>
#include <memory>
#include <string>

namespace streaming {
class EncodeScene : public gp::sdl::Scene3D {
public:
  EncodeScene(const VideoStreamInfo &video_stream_info, const int length_s);

private:
  void init(const int width, const int height, const std::string &title);

  void loop(const gp::misc::Event &event) override;

  void initialize();
  void finalize();
  void animate(const std::uint64_t time_elapsed_ms);
  void redraw();
  void encode();

  void init_streaming();
  void init_scene();

  const VideoStreamInfo video_stream_info_;
  const int number_of_frames_{};
  const int ms_per_frame_{};
  int frame_counter_{};
  std::unique_ptr<std::ofstream> output_file_{};
  std::unique_ptr<Encoder> encoder_;

  std::shared_ptr<FrameData> video_frame_{};

  glm::mat4 projection_{};
  glm::vec3 camera_pos_{};
  glm::vec3 camera_rot_{};

  std::unique_ptr<gp::gl::VertexArrayObject> vao_{};
  std::unique_ptr<gp::gl::BufferObject> vertex_buffer_{};
  std::unique_ptr<gp::gl::BufferObject> indices_buffer_{};
  std::unique_ptr<gp::gl::ShaderProgram> shader_program_{};
};
} // namespace streaming
