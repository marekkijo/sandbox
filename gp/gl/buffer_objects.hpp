#pragma once

#include <gp/gl/gl.hpp>

#include <cstddef>
#include <vector>

namespace gp::gl {
/**
 * @brief Represents a collection of OpenGL buffer objects.
 */
class BufferObjects {
public:
  /**
   * @brief Constructs a BufferObjects instance with the specified number of buffer objects and target.
   * @param n The number of buffer objects to create.
   * @param target The target buffer object type (e.g., GL_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER).
   */
  BufferObjects(const std::size_t n, const GLenum target);

  /**
   * @brief Destroys the BufferObjects instance and releases all associated buffer objects.
   */
  ~BufferObjects();

  /**
   * @brief Returns the ID of the buffer object at the specified index.
   * @param index The index of the buffer object.
   * @return The ID of the buffer object.
   */
  GLuint id(const std::size_t index) const;

  /**
   * @brief Returns the target buffer object type.
   * @return The target buffer object type.
   */
  GLenum target() const;

  /**
   * @brief Binds the buffer object at the specified index.
   * @param index The index of the buffer object to bind.
   */
  void bind(const std::size_t index) const;

  /**
   * @brief Unbinds the currently bound buffer object.
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
   * @brief Updates a subset of the buffer object's data.
   * @param offset The offset in bytes from the start of the buffer object.
   * @param size The size of the data in bytes.
   * @param data A pointer to the data.
   */
  void set_sub_data(const GLintptr offset, const GLsizeiptr size, const void *data) const;

  /**
   * @brief Retrieves a subset of the buffer object's data.
   * @param offset The offset in bytes from the start of the buffer object.
   * @param size The size of the data to retrieve in bytes.
   * @param data A pointer to the memory where the retrieved data will be stored.
   */
  void get_sub_data(const GLintptr offset, const GLsizeiptr size, void *data) const;

  /**
   * @brief Maps the buffer object's data store into the client's address space.
   * @param access The access policy for the mapped range.
   * @return A pointer to the mapped data.
   */
  void *map(const GLenum access) const;

  /**
   * @brief Maps a range of the buffer object's data store into the client's address space.
   * @param offset The offset in bytes from the start of the buffer object.
   * @param length The length of the range to map in bytes.
   * @param access The access policy for the mapped range.
   * @return A pointer to the mapped data.
   */
  void *map_range(const GLintptr offset, const GLsizeiptr length, const GLbitfield access) const;

  /**
   * @brief Flushes a mapped range of the buffer object's data store.
   * @param offset The offset in bytes from the start of the mapped range.
   * @param length The length of the mapped range in bytes.
   */
  void flush_mapped_range(const GLintptr offset, const GLsizeiptr length) const;

  /**
   * @brief Unmaps the buffer object's data store.
   */
  void unmap() const;

  /**
   * @brief Retrieves a parameter of the buffer object.
   * @param value The parameter to retrieve.
   * @param data A pointer to the memory where the retrieved parameter value will be stored.
   */
  void get_parameter(const GLenum value, GLint *data) const;

  /**
   * @brief Retrieves a parameter of the buffer object.
   * @param value The parameter to retrieve.
   * @param data A pointer to the memory where the retrieved parameter value will be stored.
   */
  void get_parameter(const GLenum value, GLint64 *data) const;

private:
  std::vector<GLuint> ids_{};
  const GLenum target_{};
};
} // namespace gp::gl
