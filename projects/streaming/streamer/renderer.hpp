#pragma once

#include "encoder.hpp"

#include "tools/sdl/sdl_animation.hpp"
#include "tools/sdl/sdl_system.hpp"

#include <glm/glm.hpp>

#include <cstdint>
#include <memory>
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

private:
  int width_{};
  int height_{};
  std::uint16_t fps_{};
  std::shared_ptr<Encoder> encoder_{};
  std::shared_ptr<std::vector<GLubyte>> gl_frame_{};
  std::unique_ptr<tools::sdl::SDLSystem> sdl_sys_{};
  std::unique_ptr<tools::sdl::SDLAnimation> animation_{};

  glm::mat4 projection_{};
  glm::vec3 camera_pos_{};
  glm::vec3 camera_rot_{};

  GLuint shader_program_{};
  GLuint viewport_location_{};
  GLuint camera_rot_location_{};

  std::thread render_thread_{};
  std::atomic_bool quit_{false};

  void render_procedure();
  void init_rendering();
  void animate(Uint32 time_elapsed_ms);
  void start_render_thread();
  void configure_program();
  [[nodiscard]] GLuint load_shader_program(const std::string &program_name);
  static void check_status(GLuint program, GLenum status);
};
} // namespace streaming
