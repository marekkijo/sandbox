#pragma once

#include <gp/glfw/glfw.hpp>

namespace gp::gl {
/**
 * @brief Represents a buffer object in OpenGL.
 */
class BufferObject {
public:
  /**
   * @brief Constructs a BufferObject with the specified target.
   * @param target The target of the buffer object.
   */
  explicit BufferObject(const GLenum target);

  /**
   * @brief Destructor for BufferObject.
   */
  ~BufferObject();

  /**
   * @brief Returns the ID of the buffer object.
   * @return The ID of the buffer object.
   */
  GLuint id() const;

  /**
   * @brief Returns the target of the buffer object.
   * @return The target of the buffer object.
   */
  GLenum target() const;

  /**
   * @brief Binds the buffer object.
   */
  void bind() const;

  /**
   * @brief Unbinds the buffer object.
   */
  void unbind() const;

  /**
   * @brief Sets the data of the buffer object.
   * @param size The size of the data in bytes.
   * @param data A pointer to the data.
   * @param usage The usage pattern of the data.
   */
  void set_data(const GLsizeiptr size, const void *data, const GLenum usage) const;

  /**
   * @brief Sets a portion of the data of the buffer object.
   * @param offset The offset in bytes.
   * @param size The size of the data in bytes.
   * @param data A pointer to the data.
   */
  void set_sub_data(const GLintptr offset, const GLsizeiptr size, const void *data) const;

  /**
   * @brief Retrieves a portion of the data from the buffer object.
   * @param offset The offset in bytes.
   * @param size The size of the data to retrieve in bytes.
   * @param data A pointer to store the retrieved data.
   */
  void get_sub_data(const GLintptr offset, const GLsizeiptr size, void *data) const;

  /**
   * @brief Maps the buffer object into client's address space.
   * @param access The access policy for the mapped range.
   * @return A pointer to the mapped range.
   */
  void *map(const GLenum access) const;

  /**
   * @brief Maps a range of the buffer object into client's address space.
   * @param offset The offset in bytes.
   * @param length The length of the range in bytes.
   * @param access The access policy for the mapped range.
   * @return A pointer to the mapped range.
   */
  void *map_range(const GLintptr offset, const GLsizeiptr length, const GLbitfield access) const;

  /**
   * @brief Flushes a mapped range of the buffer object.
   * @param offset The offset in bytes.
   * @param length The length of the range in bytes.
   */
  void flush_mapped_range(const GLintptr offset, const GLsizeiptr length) const;

  /**
   * @brief Unmaps the buffer object.
   */
  void unmap() const;

  /**
   * @brief Retrieves a parameter of the buffer object.
   * @param value The parameter to retrieve.
   * @param data A pointer to store the retrieved parameter value.
   */
  void get_parameter(const GLenum value, GLint *data) const;

  /**
   * @brief Retrieves a parameter of the buffer object.
   * @param value The parameter to retrieve.
   * @param data A pointer to store the retrieved parameter value.
   */
  void get_parameter(const GLenum value, GLint64 *data) const;

private:
  GLuint id_{};
  const GLenum target_{};
};
} // namespace gp::gl
