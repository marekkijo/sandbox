#pragma once

#include <gp/gl/gl.hpp>

#include <cstddef>
#include <vector>

namespace gp::gl {
/**
 * @brief Represents a collection of vertex array objects (VAOs) in OpenGL.
 *
 * This class provides functionality to manage and manipulate vertex array objects.
 */
class VertexArrayObjects {
public:
  /**
   * @brief Constructs a VertexArrayObjects object with the specified number of vertex array objects.
   *
   * @param n The number of vertex array objects to create.
   */
  explicit VertexArrayObjects(const std::size_t n);

  /**
   * @brief Destructor for the VertexArrayObjects class.
   */
  ~VertexArrayObjects();

  /**
   * @brief Returns the ID of the vertex array object at the specified index.
   *
   * @param index The index of the vertex array object.
   * @return The ID of the vertex array object.
   */
  GLuint id(const std::size_t index) const;

  /**
   * @brief Binds the vertex array object at the specified index.
   *
   * @param index The index of the vertex array object to bind.
   */
  void bind(const std::size_t index) const;

  /**
   * @brief Unbinds the currently bound vertex array object.
   *
   * This static function unbinds the currently bound vertex array object.
   */
  static void unbind();

private:
  std::vector<GLuint> ids_{};
};
} // namespace gp::gl
