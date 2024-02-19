#pragma once

#include <glad/glad.h>

#include <string_view>

namespace gl {
namespace buffer {
[[nodiscard]] GLuint make_vertex_buffer(GLsizeiptr size_in_bytes, const void *data, GLenum usage);
[[nodiscard]] GLuint make_vertex_buffer(GLsizeiptr size_in_bytes, const void *data);

void write_vertex_buffer_data(GLuint buffer_id, GLsizeiptr size_in_bytes, const void *data, GLenum usage);
void write_vertex_buffer_data(GLuint buffer_id, GLsizeiptr size_in_bytes, const void *data);

[[nodiscard]] GLuint make_index_buffer(GLsizeiptr size_in_bytes, const void *data, GLenum usage);
[[nodiscard]] GLuint make_index_buffer(GLsizeiptr size_in_bytes, const void *data);

void delete_buffer(GLuint &buffer_id);
} // namespace buffer

namespace shader {
[[nodiscard]] GLuint make_shader(std::string_view vert_source, std::string_view frag_source);
void delete_shader(GLuint &program_id);
} // namespace shader

namespace vao {
void delete_vao(GLuint &vao_id);
} // namespace vao

namespace texture {
void delete_texture(GLuint &tex_id);
} // namespace texture
} // namespace gl
