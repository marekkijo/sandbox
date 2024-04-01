#pragma once

#include "sprite_voxelizer.hpp"

#include <gp/gl/buffer_object.hpp>
#include <gp/gl/shader_program.hpp>
#include <gp/gl/texture_object.hpp>
#include <gp/gl/vertex_array_object.hpp>
#include <gp/glfw/glfw.hpp>
#include <gp/glfw/scene_gl.hpp>
#include <gp/misc/event_fwd.hpp>

#include <glm/glm.hpp>

#include <cstdint>
#include <memory>

class VoxelScene : public gp::glfw::SceneGL {
public:
  explicit VoxelScene(std::shared_ptr<SpriteVoxelizer> sprite_voxelizer);
  ~VoxelScene() final = default;

private:
  void loop(const gp::misc::Event &event) final;

  void initialize(const int width, const int height);
  void finalize();
  void resize(const int width, const int height);
  void animate(const std::uint32_t timestamp);
  void redraw();

  void voxelize_sprite();
  void vipe_data();
  void upload_data();

  std::shared_ptr<SpriteVoxelizer> sprite_voxelizer_{};
  std::size_t sprite_index_{};

  std::uint32_t last_timestamp_ms_{};

  glm::mat4 projection_{};
  glm::vec3 camera_pos_{};
  glm::vec3 camera_rot_{};
  bool animate_{true};

  std::unique_ptr<gp::gl::VertexArrayObject> vao_{};
  std::unique_ptr<gp::gl::BufferObject> vertex_buffer_{};
  std::unique_ptr<gp::gl::BufferObject> index_buffer_{};
  GLsizei size_{};

  std::unique_ptr<gp::gl::ShaderProgram> shader_program_{};
};
