#include "vertex_array_objects.hpp"

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace gp::gl {
VertexArrayObjects::VertexArrayObjects(const std::size_t n)
    : ids_(n) {
  glGenVertexArrays(static_cast<GLsizei>(n), ids_.data());
  if (std::any_of(ids_.begin(), ids_.end(), [](const auto id) { return id == 0; })) {
    throw std::runtime_error("Failed to generate vertex array objects");
  }
}

VertexArrayObjects::~VertexArrayObjects() { glDeleteVertexArrays(static_cast<GLsizei>(ids_.size()), ids_.data()); }

VertexArrayObjects::VertexArrayObjects(VertexArrayObjects &&other) noexcept
    : ids_(std::move(other.ids_)) {}

VertexArrayObjects &VertexArrayObjects::operator=(VertexArrayObjects &&other) noexcept {
  if (this != &other) {
    glDeleteVertexArrays(static_cast<GLsizei>(ids_.size()), ids_.data());
    ids_ = std::move(other.ids_);
  }
  return *this;
}

GLuint VertexArrayObjects::id(const std::size_t index) const { return ids_[index]; }

void VertexArrayObjects::bind(const std::size_t index) const { glBindVertexArray(id(index)); }

void VertexArrayObjects::unbind() { glBindVertexArray(0); }
} // namespace gp::gl
