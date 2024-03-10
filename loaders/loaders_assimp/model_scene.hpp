#pragma once

#include "model.hpp"

#include <gp/gl/buffer_objects.hpp>
#include <gp/gl/shader_program.hpp>
#include <gp/gl/vertex_array_objects.hpp>
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
  void finalize();
  void resize(const int width, const int height);
  void animate(const std::uint32_t timestamp);
  void redraw();

  void upload_data();

  std::shared_ptr<const Model> model_{};

  std::uint32_t last_timestamp_ms_{};

  glm::mat4 projection_{};
  glm::vec3 camera_pos_{};
  glm::vec3 camera_rot_{};
  bool animate_{true};

  std::size_t number_of_meshes_{};
  std::unique_ptr<gp::gl::VertexArrayObjects> vaos_{};
  std::unique_ptr<gp::gl::BufferObjects> vertices_buffers_{};
  std::unique_ptr<gp::gl::BufferObjects> normals_buffers_{};
  std::unique_ptr<gp::gl::BufferObjects> indices_buffers_{};
  std::vector<glm::vec4> colors_{};
  std::vector<GLsizei> sizes_{};

  std::unique_ptr<gp::gl::ShaderProgram> shader_program_{};
};
