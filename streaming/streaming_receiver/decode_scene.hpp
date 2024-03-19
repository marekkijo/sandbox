#pragma once

#include "streaming_common/decoder_fwd.hpp"
#include "streaming_common/frame_data.hpp"
#include "streaming_common/video_stream_info.hpp"

#include <gp/gl/buffer_object.hpp>
#include <gp/gl/shader_program.hpp>
#include <gp/gl/texture_object.hpp>
#include <gp/gl/vertex_array_object.hpp>
#include <gp/glfw/scene_gl.hpp>
#include <gp/misc/event.hpp>

#include <cstdint>
#include <functional>
#include <memory>
#include <string>

namespace streaming {
class DecodeScene : public gp::glfw::SceneGL {
public:
  DecodeScene();
  ~DecodeScene() final;

  void init(const VideoStreamInfo &video_stream_info);
  void consume_data(const std::byte *data, const std::size_t size);

  void set_event_callback(std::function<void(const gp::misc::Event &event)> event_callback);

private:
  void init(const int width, const int height, const std::string &title);

  void loop(const gp::misc::Event &event) final;

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
  bool frame_ready_{};

  std::unique_ptr<gp::gl::VertexArrayObject> vao_{};
  std::unique_ptr<gp::gl::BufferObject> vertex_buffer_{};
  std::unique_ptr<gp::gl::TextureObject> frame_texture_{};
  std::unique_ptr<gp::gl::ShaderProgram> shader_program_{};

  std::function<void(const gp::misc::Event &event)> event_callback_{};
};
} // namespace streaming
