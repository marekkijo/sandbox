#include "gl.hpp"

#include <fmt/format.h>

#include <array>
#include <cassert>

namespace gl {
namespace buffer {
namespace {
void write_buffer_data(const GLenum buffer_type,
                       const GLuint buffer_id,
                       const GLsizeiptr size_in_bytes,
                       const void *data,
                       GLenum usage) {
  assert((buffer_type == GL_ARRAY_BUFFER || buffer_type == GL_ELEMENT_ARRAY_BUFFER) && "invalid buffer type");
  assert((usage == GL_STATIC_DRAW || usage == GL_DYNAMIC_DRAW || usage == GL_STATIC_COPY || usage == GL_DYNAMIC_COPY ||
          usage == GL_STATIC_READ || usage == GL_DYNAMIC_READ) &&
         "invalid usage");

  glBindBuffer(buffer_type, buffer_id);
  glBufferData(buffer_type, size_in_bytes, data, usage);
  glBindBuffer(buffer_type, 0);
}

GLuint make_buffer(const GLenum buffer_type, const GLsizeiptr size_in_bytes, const void *data, const GLenum usage) {
  GLuint id{0U};
  glGenBuffers(1, &id);
  write_buffer_data(buffer_type, id, size_in_bytes, data, usage);
  return id;
}
} // namespace

GLuint make_vertex_buffer(const GLsizeiptr size_in_bytes, const void *data, const GLenum usage) {
  return make_buffer(GL_ARRAY_BUFFER, size_in_bytes, data, usage);
}

GLuint make_vertex_buffer(const GLsizeiptr size_in_bytes, const void *data) {
  return make_vertex_buffer(size_in_bytes, data, GL_STATIC_DRAW);
}

void write_vertex_buffer_data(const GLuint buffer_id, const GLsizeiptr size_in_bytes, const void *data, GLenum usage) {
  write_buffer_data(GL_ARRAY_BUFFER, buffer_id, size_in_bytes, data, usage);
}

void write_vertex_buffer_data(const GLuint buffer_id, const GLsizeiptr size_in_bytes, const void *data) {
  write_vertex_buffer_data(buffer_id, size_in_bytes, data, GL_STATIC_DRAW);
}

GLuint make_index_buffer(const GLsizeiptr size_in_bytes, const void *data, const GLenum usage) {
  return make_buffer(GL_ELEMENT_ARRAY_BUFFER, size_in_bytes, data, usage);
}

GLuint make_index_buffer(const GLsizeiptr size_in_bytes, const void *data) {
  return make_index_buffer(size_in_bytes, data, GL_STATIC_DRAW);
}

void delete_buffer(GLuint &buffer_id) {
  if (buffer_id != 0U) {
    glDeleteBuffers(1, &buffer_id);
    buffer_id = 0U;
  }
}
} // namespace buffer

namespace shader {
namespace {
void verify_shader(const GLuint shader_id) {
  GLint success = 0;
  glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
  if (!success) // NOLINT(readability-implicit-bool-conversion)
  {
    std::array<char, 512UL> info;
    glGetShaderInfoLog(shader_id, info.size(), nullptr, info.data());
    throw std::runtime_error(fmt::format("shader compilation failed:\n{}\n", info.data()));
  }
}

void verify_program(const GLuint program_id) {
  GLint success = 0;
  glGetProgramiv(program_id, GL_LINK_STATUS, &success);
  if (!success) // NOLINT(readability-implicit-bool-conversion)
  {
    std::array<char, 512UL> info;
    glGetProgramInfoLog(program_id, info.size(), nullptr, info.data());
    throw std::runtime_error(fmt::format("shader program linking failed:\n{}\n", info.data()));
  }
}
} // namespace

GLuint make_shader(const std::string_view vert_source, const std::string_view frag_source) {
  const GLuint vert_id = glCreateShader(GL_VERTEX_SHADER);
  const auto *c_vert_src = vert_source.data();
  glShaderSource(vert_id, 1, &c_vert_src, nullptr);
  glCompileShader(vert_id);
  verify_shader(vert_id);

  const GLuint frag_id = glCreateShader(GL_FRAGMENT_SHADER);
  const auto *c_frag_src = frag_source.data();
  glShaderSource(frag_id, 1, &c_frag_src, nullptr);
  glCompileShader(frag_id);
  verify_shader(frag_id);

  GLuint program_id = glCreateProgram();
  glAttachShader(program_id, vert_id);
  glAttachShader(program_id, frag_id);
  glLinkProgram(program_id);
  verify_program(program_id);

  glDeleteShader(vert_id);
  glDeleteShader(frag_id);

  return program_id;
}

void delete_shader(GLuint &program_id) {
  if (program_id != 0U) {
    glDeleteProgram(program_id);
    program_id = 0U;
  }
}
} // namespace shader

namespace vao {
void delete_vao(GLuint &vao_id) {
  if (vao_id != 0U) {
    glDeleteVertexArrays(1, &vao_id);
    vao_id = 0U;
  }
}
} // namespace vao

namespace texture {
void delete_texture(GLuint &tex_id) {
  if (tex_id != 0U) {
    glDeleteTextures(1, &tex_id);
    tex_id = 0U;
  }
}
} // namespace texture
} // namespace gl
