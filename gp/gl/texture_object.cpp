#include "texture_object.hpp"

#include <stdexcept>

namespace gp::gl {
TextureObject::TextureObject(const GLenum target)
    : target_(target) {
  glGenTextures(1, &id_);
  if (id() == 0) {
    throw std::runtime_error("Failed to generate texture object");
  }
}

TextureObject::~TextureObject() { glDeleteTextures(1, &id_); }

GLuint TextureObject::id() const { return id_; }

GLenum TextureObject::target() const { return target_; }

void TextureObject::bind() const { glBindTexture(target(), id()); }

void TextureObject::unbind() const { glBindTexture(target(), 0); }

void TextureObject::set_image(const GLint level,
                              const GLint internal_format,
                              const GLsizei width,
                              const GLsizei height,
                              const GLint border,
                              const GLenum format,
                              const GLenum type,
                              const void *data) const {
  glTexImage2D(target(), level, internal_format, width, height, border, format, type, data);
}

void TextureObject::set_sub_image(const GLint level,
                                  const GLint xoffset,
                                  const GLint yoffset,
                                  const GLsizei width,
                                  const GLsizei height,
                                  const GLenum format,
                                  const GLenum type,
                                  const void *data) const {
  glTexSubImage2D(target(), level, xoffset, yoffset, width, height, format, type, data);
}

void TextureObject::set_parameter(const GLenum pname, const GLfloat param) const {
  glTexParameterf(target(), pname, param);
}

void TextureObject::set_parameter(const GLenum pname, const GLint param) const {
  glTexParameteri(target(), pname, param);
}

void TextureObject::set_parameter(const GLenum pname, const GLfloat *params) const {
  glTexParameterfv(target(), pname, params);
}

void TextureObject::set_parameter(const GLenum pname, const GLint *params) const {
  glTexParameteriv(target(), pname, params);
}

void TextureObject::set_parameter_I(const GLenum pname, const GLint *params) const {
  glTexParameterIiv(target(), pname, params);
}

void TextureObject::set_parameter_I(const GLenum pname, const GLuint *params) const {
  glTexParameterIuiv(target(), pname, params);
}

void TextureObject::get_parameter(const GLenum pname, GLfloat *params) const {
  glGetTexParameterfv(target(), pname, params);
}

void TextureObject::get_parameter(const GLenum pname, GLint *params) const {
  glGetTexParameteriv(target(), pname, params);
}

void TextureObject::get_parameter_I(const GLenum pname, GLint *params) const {
  glGetTexParameterIiv(target(), pname, params);
}

void TextureObject::get_parameter_I(const GLenum pname, GLuint *params) const {
  glGetTexParameterIuiv(target(), pname, params);
}
} // namespace gp::gl
