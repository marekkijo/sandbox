#include "buffer_objects.hpp"

#include <algorithm>
#include <stdexcept>

namespace gp::gl {
BufferObjects::BufferObjects(const std::size_t n, const GLenum target)
    : ids_(n)
    , target_(target) {
  glGenBuffers(static_cast<GLsizei>(n), ids_.data());
  if (std::any_of(ids_.begin(), ids_.end(), [](const auto id) { return id == 0; })) {
    throw std::runtime_error("Failed to generate buffer objects");
  }
}

BufferObjects::~BufferObjects() { glDeleteBuffers(static_cast<GLsizei>(ids_.size()), ids_.data()); }

GLuint BufferObjects::id(const std::size_t index) const { return ids_[index]; }

GLenum BufferObjects::target() const { return target_; }

void BufferObjects::bind(const std::size_t index) const { glBindBuffer(target(), id(index)); }

void BufferObjects::unbind() const { glBindBuffer(target(), 0); }

void BufferObjects::set_data(const GLsizeiptr size, const void *data, const GLenum usage) const {
  glBufferData(target(), size, data, usage);
}

void BufferObjects::set_sub_data(const GLintptr offset, const GLsizeiptr size, const void *data) const {
  glBufferSubData(target(), offset, size, data);
}

void BufferObjects::get_sub_data(const GLintptr offset, const GLsizeiptr size, void *data) const {
#ifndef __EMSCRIPTEN__
  glGetBufferSubData(target(), offset, size, data);
#else
  throw std::runtime_error("glGetBufferSubData is not available in Emscripten");
#endif
}

void *BufferObjects::map(const GLenum access) const {
#ifndef __EMSCRIPTEN__
  return glMapBuffer(target(), access);
#else
  throw std::runtime_error("glMapBuffer is not available in Emscripten");
#endif
}

void *BufferObjects::map_range(const GLintptr offset, const GLsizeiptr length, const GLbitfield access) const {
  return glMapBufferRange(target(), offset, length, access);
}

void BufferObjects::flush_mapped_range(const GLintptr offset, const GLsizeiptr length) const {
  glFlushMappedBufferRange(target(), offset, length);
}

void BufferObjects::unmap() const { glUnmapBuffer(target()); }

void BufferObjects::get_parameter(const GLenum value, GLint *data) const {
  glGetBufferParameteriv(target(), value, data);
}

void BufferObjects::get_parameter(const GLenum value, GLint64 *data) const {
  glGetBufferParameteri64v(target(), value, data);
}
} // namespace gp::gl
