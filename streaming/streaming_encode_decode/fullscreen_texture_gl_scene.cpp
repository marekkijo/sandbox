#include "fullscreen_texture_gl_scene.hpp"

#include <array>
#include <string_view>

#include "gl.hpp"

FullscreenTextureGLScene::~FullscreenTextureGLScene() {
  gl::buffer::delete_buffer(vbo_);
  gl::vao::delete_vao(vao_);
  gl::shader::delete_shader(shader_program_);
  if (owns_tex_) { gl::texture::delete_texture(tex_id_); }
}

void FullscreenTextureGLScene::init() {
  init_buffer();
  init_shader();

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void FullscreenTextureGLScene::draw() {
  glViewport(0, 0, window_width(), window_height());
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, tex_id_);

  glUseProgram(shader_program_);
  glBindVertexArray(vao_);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);
  glUseProgram(0);

  glBindTexture(GL_TEXTURE_2D, 0);
}

void FullscreenTextureGLScene::set_texture(const GLuint tex_id, const bool take_ownership) {
  tex_id_ = tex_id;
  owns_tex_ = take_ownership;
}

GLuint FullscreenTextureGLScene::texture_id() const { return tex_id_; }

void FullscreenTextureGLScene::init_buffer() {
  constexpr auto vertex_data = std::array<GLfloat, 16u>{-1.0f,
                                                        1.0f,
                                                        0.0f,
                                                        0.0f,
                                                        -1.0f,
                                                        -1.0f,
                                                        0.0f,
                                                        1.0f,
                                                        1.0f,
                                                        1.0f,
                                                        1.0f,
                                                        0.0f,
                                                        1.0f,
                                                        -1.0f,
                                                        1.0f,
                                                        1.0f};
  vbo_ = gl::buffer::make_vertex_buffer(vertex_data.size() * sizeof(GLfloat), vertex_data.data());

  constexpr GLsizei stride_in_bytes{4 * sizeof(GLfloat)};

  glGenVertexArrays(1, &vao_);
  glBindVertexArray(vao_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride_in_bytes, reinterpret_cast<void *>(0));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride_in_bytes, reinterpret_cast<void *>(2 * sizeof(GLfloat)));
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void FullscreenTextureGLScene::init_shader() {
  constexpr std::string_view vert_src = "#version 330 core\n"
                                        ""
                                        "layout(location = 0) in vec2 pos_in;"
                                        "layout(location = 1) in vec2 texcoord_in;"
                                        ""
                                        "out vec2 texcoord_frag;"
                                        ""
                                        "void main()"
                                        "{"
                                        "  gl_Position = vec4(pos_in, 0, 1);"
                                        "  texcoord_frag = texcoord_in;"
                                        "}";

  constexpr std::string_view frag_src = "#version 330 core\n"
                                        ""
                                        "#ifdef GL_ES\n"
                                        "precision mediump float;\n"
                                        "#endif\n"
                                        ""
                                        "in vec2 texcoord_frag;"
                                        ""
                                        "uniform sampler2D tex;"
                                        ""
                                        "out vec4 frag_color;"
                                        ""
                                        "void main()"
                                        "{"
                                        "  frag_color = texture2D(tex, texcoord_frag);"
                                        "}";

  shader_program_ = gl::shader::make_shader(vert_src, frag_src);
}
