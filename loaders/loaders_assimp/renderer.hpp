#pragma once

#include "model.hpp"

#include <gp/common/user_input.hpp>
#include <gp/sdl/animation.hpp>
#include <gp/sdl/system.hpp>

#include <glm/glm.hpp>

#include <cstdint>
#include <memory>
#include <mutex>
#include <thread>

class Renderer {
public:
  Renderer(const Renderer &) = delete;
  Renderer &operator=(const Renderer &) = delete;
  Renderer(Renderer &&other) noexcept = delete;
  Renderer &operator=(Renderer &&other) noexcept = delete;

  Renderer(int width, int height, std::uint16_t fps, std::shared_ptr<const Model> &model);
  ~Renderer();

private:
  int width_{};
  int height_{};
  std::uint16_t fps_{};
  std::shared_ptr<const Model> model_{};
  std::unique_ptr<gp::sdl::System> sdl_sys_{};
  std::unique_ptr<gp::sdl::Animation> animation_{};

  glm::mat4 projection_{};
  glm::vec3 camera_pos_{};
  glm::vec3 camera_rot_{};
  bool animate_{true};

  std::vector<GLuint> vertices_bufs_{};
  std::vector<GLuint> indices_bufs_{};
  std::vector<glm::vec4> colors_{};
  std::vector<GLsizei> sizes_{};
  GLuint shader_program_{};
  GLuint color_location_{};
  GLuint viewport_location_{};
  GLuint camera_rot_location_{};

  std::thread render_thread_{};
  std::atomic_bool quit_{false};

  std::mutex mutex_{};

  void start_render_thread();
  void process_user_input(const gp::common::UserInput &user_input);

  void render_procedure();
  void init_rendering();
  void animate(Uint32 time_elapsed_ms);
  void configure_program();
  [[nodiscard]] GLuint load_shader_program(const std::string &program_name);
  static void check_shader_status(GLuint shader, GLenum status);
  static void check_program_status(GLuint program, GLenum status);
};
