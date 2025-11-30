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
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace streaming {
class EncodeScene : public gp::sdl::Scene3D {
public:
  explicit EncodeScene(const VideoStreamInfo &video_stream_info);

  std::shared_ptr<Encoder> encoder() const;
  void handle_event(const gp::misc::Event &event);

private:
  void init(const int width, const int height, const std::string &title);

  void loop(const gp::misc::Event &event) override;

  void initialize();
  void finalize();
  void process_event_queue();
  void animate(const std::uint64_t time_elapsed_ms);
  void redraw();
  void encode();

  void init_scene();

  std::shared_ptr<Encoder> encoder_;
  const int ms_per_frame_{};
  std::uint64_t last_timestamp_ms_{};

  std::shared_ptr<FrameData> video_frame_{};

  bool animate_{};
  glm::mat4 projection_{};
  glm::vec3 camera_pos_{};
  glm::vec3 camera_rot_{};

  std::unique_ptr<gp::gl::VertexArrayObject> vao_{};
  std::unique_ptr<gp::gl::BufferObject> vertex_buffer_{};
  std::unique_ptr<gp::gl::BufferObject> indices_buffer_{};
  std::unique_ptr<gp::gl::ShaderProgram> shader_program_{};

  std::vector<gp::misc::Event> event_queue_{};
  std::mutex event_queue_mutex_{};
};
} // namespace streaming
