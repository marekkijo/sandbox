#include "vertex_array_object.hpp"

#include <stdexcept>

namespace gp::gl {
VertexArrayObject::VertexArrayObject() {
  glGenVertexArrays(1, &id_);
  if (id() == 0) { throw std::runtime_error("Failed to generate vertex array object"); }
}

VertexArrayObject::~VertexArrayObject() { glDeleteVertexArrays(1, &id_); }

GLuint VertexArrayObject::id() const { return id_; }

void VertexArrayObject::bind() const { glBindVertexArray(id()); }

void VertexArrayObject::unbind() { glBindVertexArray(0); }
} // namespace gp::gl
