#pragma once

#include "gl_scene.hpp"

#include <glm/glm.hpp>

#include <mutex>

class RGBCubeGLScene final : public GLScene {
public:
  using GLScene::GLScene;
  RGBCubeGLScene() = delete;
  RGBCubeGLScene(const RGBCubeGLScene &) = delete;
  RGBCubeGLScene &operator=(const RGBCubeGLScene &) = delete;
  RGBCubeGLScene(RGBCubeGLScene &&) noexcept = delete;
  RGBCubeGLScene &operator=(RGBCubeGLScene &&) noexcept = delete;
  ~RGBCubeGLScene() final;

  void init() final;
  void draw() final;
  void process_event(const gp::misc::Event &event) final;

  void toggle_animation_speed_factor();

private:
  void init_buffer();
  void init_shader();
  void animate(int time_elapsed_ms);

  GLuint vbo_{0U};
  GLuint ibo_{0U};
  GLsizei num_indices_{0};
  GLuint vao_{0U};
  GLuint shader_program_{0U};
  GLint mvp_location_{0};
  std::uint16_t fps_{};

  glm::mat4 projection_{};
  glm::mat4 view_{};
  glm::mat4 viewport_mat_{};
  glm::vec3 camera_pos_{};
  glm::vec3 camera_rot_{};

  bool animate_{true};
  float animation_speed_factor_{0.01f};

  std::mutex mutex_{};
};
