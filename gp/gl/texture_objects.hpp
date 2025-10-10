#pragma once

#include <gp/gl/gl.hpp>

#include <cstddef>
#include <vector>

namespace gp::gl {
/**
 * @brief Represents a collection of OpenGL texture objects.
 */
class TextureObjects {
public:
  /**
   * @brief Constructs a TextureObjects instance with the specified number of texture objects and target.
   * @param n The number of texture objects to create.
   * @param target The target texture object type (e.g., GL_TEXTURE_2D, GL_TEXTURE_2D_ARRAY).
   */
  TextureObjects(const std::size_t n, const GLenum target);

  /**
   * @brief Destroys the TextureObjects instance and releases all associated texture objects.
   */
  ~TextureObjects();

  /**
   * @brief Returns the ID of the texture object at the specified index.
   * @param index The index of the texture object.
   * @return The ID of the texture object.
   */
  GLuint id(const std::size_t index) const;

  /**
   * @brief Returns the target texture object type.
   * @return The target texture object type.
   */
  GLenum target() const;

  /**
   * @brief Binds the texture object at the specified index.
   * @param index The index of the texture object to bind.
   */
  void bind(const std::size_t index) const;

  /**
   * @brief Unbinds the currently bound texture object.
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
   * @param xoffset The x offset of the sub-region.
   * @param yoffset The y offset of the sub-region.
   * @param width The width of the sub-region.
   * @param height The height of the sub-region.
   * @param format The format of the pixel data.
   * @param type The data type of the pixel data.
   * @param data A pointer to the image data in memory.
   */
  void set_sub_image(const GLint level,
                     const GLint xoffset,
                     const GLint yoffset,
                     const GLsizei width,
                     const GLsizei height,
                     const GLenum format,
                     const GLenum type,
                     const void *data) const;

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
  std::vector<GLuint> ids_{};
  const GLenum target_{};
};
} // namespace gp::gl
