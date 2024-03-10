#include "buffer_object.hpp"

#include <stdexcept>

namespace gp::gl {
BufferObject::BufferObject(const GLenum target)
    : target_(target) {
  glGenBuffers(1, &id_);
  if (id() == 0) { throw std::runtime_error("Failed to generate buffer object"); }
}

BufferObject::~BufferObject() { glDeleteBuffers(1, &id_); }

GLuint BufferObject::id() const { return id_; }

GLenum BufferObject::target() const { return target_; }

void BufferObject::bind() const { glBindBuffer(target(), id()); }

void BufferObject::unbind() const { glBindBuffer(target(), 0); }

void BufferObject::set_data(const GLsizeiptr size, const void *data, const GLenum usage) const {
  glBufferData(target(), size, data, usage);
}

void BufferObject::set_sub_data(const GLintptr offset, const GLsizeiptr size, const void *data) const {
  glBufferSubData(target(), offset, size, data);
}

void BufferObject::get_sub_data(const GLintptr offset, const GLsizeiptr size, void *data) const {
  glGetBufferSubData(target(), offset, size, data);
}

void *BufferObject::map(const GLenum access) const { return glMapBuffer(target(), access); }

void *BufferObject::map_range(const GLintptr offset, const GLsizeiptr length, const GLbitfield access) const {
  return glMapBufferRange(target(), offset, length, access);
}

void BufferObject::flush_mapped_range(const GLintptr offset, const GLsizeiptr length) const {
  glFlushMappedBufferRange(target(), offset, length);
}

void BufferObject::unmap() const { glUnmapBuffer(target()); }

void BufferObject::get_parameter(const GLenum value, GLint *data) const {
  glGetBufferParameteriv(target(), value, data);
}

void BufferObject::get_parameter(const GLenum value, GLint64 *data) const {
  glGetBufferParameteri64v(target(), value, data);
}
} // namespace gp::gl
