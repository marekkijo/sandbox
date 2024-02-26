#pragma once

#include "model.hpp"

#include <gp/glfw/glfw.hpp>
#include <gp/glfw/scene_gl.hpp>
#include <gp/misc/event_fwd.hpp>

#include <glm/glm.hpp>

#include <cstdint>
#include <memory>

class ModelScene : public gp::glfw::SceneGL {
public:
  explicit ModelScene(std::shared_ptr<const Model> model);
  ~ModelScene() final = default;

  void loop(const gp::misc::Event &event) final;

private:
  void initialize(const int width, const int height);
  void resize(const int width, const int height);
  void animate(const std::uint32_t timestamp);
  void redraw();

  void configure_program();

  std::shared_ptr<const Model> model_{};

  std::uint32_t last_timestamp_ms_{};

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

  [[nodiscard]] GLuint load_shader_program(const std::string &program_name);
  static void check_shader_status(GLuint shader, GLenum status);
  static void check_program_status(GLuint program, GLenum status);
};
