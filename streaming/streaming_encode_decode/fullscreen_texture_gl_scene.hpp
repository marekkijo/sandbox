#pragma once

#include "gl_scene.hpp"

class FullscreenTextureGLScene final : public GLScene {
public:
  using GLScene::GLScene;
  FullscreenTextureGLScene() = delete;
  FullscreenTextureGLScene(const FullscreenTextureGLScene &) = delete;
  FullscreenTextureGLScene &operator=(const FullscreenTextureGLScene &) = delete;
  FullscreenTextureGLScene(FullscreenTextureGLScene &&) noexcept = delete;
  FullscreenTextureGLScene &operator=(FullscreenTextureGLScene &&) noexcept = delete;
  ~FullscreenTextureGLScene() final;

  void init() final;
  void draw() final;

  /// @param[in] tex_id id of the already created texture
  /// @param[in] take_ownership if @c true, this class will delete the texture in its destructor
  void set_texture(GLuint tex_id, bool take_ownership = true);
  [[nodiscard]] GLuint texture_id() const;

private:
  void init_buffer();
  void init_shader();

  GLuint vbo_{0U};
  GLuint vao_{0U};
  GLuint shader_program_{0U};

  GLuint tex_id_{0U};
  bool owns_tex_{false};
};
