#pragma once

#include <gp/glfw/glfw.hpp>

namespace gp::gl {
/**
 * @brief Represents a Vertex Array Object (VAO) in OpenGL.
 *
 * The VertexArrayObject class encapsulates the functionality of a Vertex Array Object,
 * which is used to store the configuration of vertex attribute pointers and vertex buffer bindings.
 * It provides methods to create, bind, and unbind a VAO.
 */
class VertexArrayObject {
public:
  /**
   * @brief Constructs a new VertexArrayObject.
   */
  VertexArrayObject();

  /**
   * @brief Destroys the VertexArrayObject and releases any associated resources.
   */
  ~VertexArrayObject();

  /**
   * @brief Returns the ID of the VertexArrayObject.
   * @return The ID of the VertexArrayObject.
   */
  GLuint id() const;

  /**
   * @brief Binds the VertexArrayObject.
   *
   * Binding a VAO makes it the active VAO, so any subsequent OpenGL operations
   * related to vertex attribute pointers and vertex buffer bindings will affect this VAO.
   */
  void bind() const;

  /**
   * @brief Unbinds the currently bound VertexArrayObject.
   *
   * This static method unbinds the currently bound VAO, making no VAO active.
   * Subsequent OpenGL operations related to vertex attribute pointers and vertex buffer bindings
   * will not affect any VAO until another VAO is bound.
   */
  static void unbind();

private:
  GLuint id_{};
};
} // namespace gp::gl
