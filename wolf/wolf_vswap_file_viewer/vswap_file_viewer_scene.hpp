#pragma once

#include "wolf_common/vswap_file.hpp"

#include <gp/gl/buffer_object.hpp>
#include <gp/gl/shader_program.hpp>
#include <gp/gl/texture_object.hpp>
#include <gp/gl/vertex_array_object.hpp>
#include <gp/misc/event.hpp>
#include <gp/sdl/scene_3d.hpp>

#include <glm/glm.hpp>

#include <memory>

namespace wolf {
class VswapFileViewerScene : public gp::sdl::Scene3D {
public:
  explicit VswapFileViewerScene(std::shared_ptr<const VswapFile> vswap_file);

private:
  void loop(const gp::misc::Event &event) override;

  void initialize(const int width, const int height);
  void finalize();
  void resize(const int width, const int height);
  void redraw();

  void upload_wall();
  void upload_sprite();

  std::shared_ptr<const VswapFile> vswap_file_{};

  std::size_t wall_index_{};
  std::size_t sprite_index_{};

  std::unique_ptr<gp::gl::VertexArrayObject> vao_{};
  std::unique_ptr<gp::gl::BufferObject> vertex_buffer_{};
  std::unique_ptr<gp::gl::TextureObject> frame_texture_{};
  std::unique_ptr<gp::gl::ShaderProgram> shader_program_{};
};
} // namespace wolf
