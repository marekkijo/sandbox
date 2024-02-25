#pragma once

#include "streaming_common/encoder.hpp"

#include <gp/glfw/glfw.hpp>
#include <gp/misc/event.hpp>
#include <gp/sdl/animation.hpp>
#include <gp/sdl/system.hpp>

#include <glm/glm.hpp>

#include <cstdint>
#include <memory>
#include <mutex>
#include <thread>

namespace streaming {
class Renderer {
public:
  Renderer(const Renderer &) = delete;
  Renderer &operator=(const Renderer &) = delete;
  Renderer(Renderer &&other) noexcept = delete;
  Renderer &operator=(Renderer &&other) noexcept = delete;

  Renderer(int width, int height, std::uint16_t fps, std::shared_ptr<Encoder> &encoder);
  ~Renderer();

  void start_render_thread();
  void process_event(const gp::misc::Event &event);

private:
  void render_procedure();
  void init_rendering();
  void animate(Uint32 time_elapsed_ms);
  void configure_program();
  [[nodiscard]] GLuint load_shader_program(const std::string &program_name);
  static void check_status(GLuint program, GLenum status);

  int width_{};
  int height_{};
  std::uint16_t fps_{};
  std::shared_ptr<Encoder> encoder_{};
  std::shared_ptr<std::vector<std::byte>> video_frame_{};
  std::unique_ptr<gp::sdl::System> sdl_sys_{};
  std::unique_ptr<gp::sdl::Animation> animation_{};

  glm::mat4 projection_{};
  glm::vec3 camera_pos_{};
  glm::vec3 camera_rot_{};
  bool animate_{true};

  GLuint shader_program_{};
  GLuint viewport_location_{};
  GLuint camera_rot_location_{};

  std::thread render_thread_{};
  std::atomic_bool quit_{false};

  std::mutex mutex_{};
};
} // namespace streaming
