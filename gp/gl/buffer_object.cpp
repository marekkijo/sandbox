#include "buffer_object.hpp"

#include <stdexcept>

namespace gp::gl {
BufferObject::BufferObject(const GLenum target)
    : target_(target) {
  glGenBuffers(1, &id_);
  if (id() == 0) {
    throw std::runtime_error("Failed to generate buffer object");
  }
}

BufferObject::~BufferObject() { glDeleteBuffers(1, &id_); }

BufferObject::BufferObject(BufferObject &&other) noexcept
    : id_{other.id_}
    , target_{other.target_} {
  other.id_ = 0;
}

BufferObject &BufferObject::operator=(BufferObject &&other) noexcept {
  if (this != &other) {
    glDeleteBuffers(1, &id_);
    id_ = other.id_;
    target_ = other.target_;
    other.id_ = 0;
  }
  return *this;
}

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
#ifndef __EMSCRIPTEN__
  glGetBufferSubData(target(), offset, size, data);
#else
  throw std::runtime_error("glGetBufferSubData is not available in Emscripten");
#endif
}

void *BufferObject::map(const GLenum access) const {
#ifndef __EMSCRIPTEN__
  return glMapBuffer(target(), access);
#else
  throw std::runtime_error("glMapBuffer is not available in Emscripten");
#endif
}

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
