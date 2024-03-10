#pragma once

#include <gp/glfw/glfw.hpp>

#include <cstddef>

namespace gp::gl {
/**
 * @brief Represents a texture object in OpenGL.
 */
class TextureObject {
public:
  /**
   * @brief Constructs a TextureObject with the specified target.
   * @param target The target of the texture object.
   */
  explicit TextureObject(const GLenum target);

  /**
   * @brief Destructor for TextureObject.
   */
  ~TextureObject();

  /**
   * @brief Returns the ID of the texture object.
   * @return The ID of the texture object.
   */
  GLuint id() const;

  /**
   * @brief Returns the target of the texture object.
   * @return The target of the texture object.
   */
  GLenum target() const;

  /**
   * @brief Binds the texture object.
   */
  void bind() const;

  /**
   * @brief Unbinds the texture object.
   */
  void unbind() const;

  /**
   * @brief Sets the data of the texture object.
   * @param level The level-of-detail number.
   * @param internal_format The number of color components in the texture.
   * @param width The width of the texture image.
   * @param height The height of the texture image.
   * @param border The width of the border. Must be 0.
   * @param format The format of the pixel data.
   * @param type The data type of the pixel data.
   * @param data A pointer to the image data in memory.
   */
  void set_image(const GLint level,
                 const GLint internal_format,
                 const GLsizei width,
                 const GLsizei height,
                 const GLint border,
                 const GLenum format,
                 const GLenum type,
                 const void *data) const;

  /**
   * @brief Sets the data of a portion of the texture object.
   * @param level The level-of-detail number.
   * @param x_offset The x offset of the sub-region.
   * @param y_offset The y offset of the sub-region.
   * @param width The width of the sub-region.
   * @param height The height of the sub-region.
   * @param format The format of the pixel data.
   * @param type The data type of the pixel data.
   * @param data A pointer to the image data in memory.
   */
  void set_sub_image(const GLint level,
                     const GLint x_offset,
                     const GLint y_offset,
                     const GLsizei width,
                     const GLsizei height,
                     const GLenum format,
                     const GLenum type,
                     const void *data) const;

  /**
   * @brief Retrieves a portion of the data from the texture object.
   * @param level The level-of-detail number.
   * @param x_offset The x offset of the sub-region.
   * @param y_offset The y offset of the sub-region.
   * @param width The width of the sub-region.
   * @param height The height of the sub-region.
   * @param format The format of the pixel data.
   * @param type The data type of the pixel data.
   * @param data A pointer to store the retrieved data.
   */
  void get_image(const GLint level, const GLenum format, const GLenum type, void *data) const;

  /**
   * @brief Sets the value of a texture parameter.
   * @param pname The name of the parameter.
   * @param param The value of the parameter.
   * @{
   */
  void set_parameter(const GLenum pname, const GLfloat param) const;
  void set_parameter(const GLenum pname, const GLint param) const;
  void set_parameter(const GLenum pname, const GLfloat *param) const;
  void set_parameter(const GLenum pname, const GLint *param) const;
  void set_parameter_I(const GLenum pname, const GLint *param) const;
  void set_parameter_I(const GLenum pname, const GLuint *param) const;
  /**
   * @}
   */

  /**
   * @brief Retrieves the value of a texture parameter.
   * @param pname The name of the parameter.
   * @param params A pointer to store the retrieved parameter value.
   * @{
   */
  void get_parameter(const GLenum pname, GLfloat *params) const;
  void get_parameter(const GLenum pname, GLint *params) const;
  void get_parameter_I(const GLenum pname, GLint *params) const;
  void get_parameter_I(const GLenum pname, GLuint *params) const;
  /**
   * @}
   */

private:
  GLuint id_{};
  const GLenum target_{};
};
} // namespace gp::gl
