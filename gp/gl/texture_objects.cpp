#include "texture_objects.hpp"

#include <algorithm>
#include <stdexcept>

namespace gp::gl {
TextureObjects::TextureObjects(const std::size_t n, const GLenum target)
    : ids_(n)
    , target_(target) {
  glGenTextures(static_cast<GLsizei>(n), ids_.data());
  if (std::any_of(ids_.begin(), ids_.end(), [](const auto id) { return id == 0; })) {
    throw std::runtime_error("Failed to generate texture objects");
  }
}

TextureObjects::~TextureObjects() { glDeleteTextures(static_cast<GLsizei>(ids_.size()), ids_.data()); }

GLuint TextureObjects::id(const std::size_t index) const { return ids_[index]; }

GLenum TextureObjects::target() const { return target_; }

void TextureObjects::bind(const std::size_t index) const { glBindTexture(target(), id(index)); }

void TextureObjects::unbind() const { glBindTexture(target(), 0); }

void TextureObjects::set_image(const GLint level,
                               const GLint internal_format,
                               const GLsizei width,
                               const GLsizei height,
                               const GLint border,
                               const GLenum format,
                               const GLenum type,
                               const void *data) const {
  glTexImage2D(target(), level, internal_format, width, height, border, format, type, data);
}

void TextureObjects::set_sub_image(const GLint level,
                                   const GLint xoffset,
                                   const GLint yoffset,
                                   const GLsizei width,
                                   const GLsizei height,
                                   const GLenum format,
                                   const GLenum type,
                                   const void *data) const {
  glTexSubImage2D(target(), level, xoffset, yoffset, width, height, format, type, data);
}

void TextureObjects::set_parameter(const GLenum pname, const GLfloat param) const {
  glTexParameterf(target(), pname, param);
}

void TextureObjects::set_parameter(const GLenum pname, const GLint param) const {
  glTexParameteri(target(), pname, param);
}

void TextureObjects::set_parameter(const GLenum pname, const GLfloat *params) const {
  glTexParameterfv(target(), pname, params);
}

void TextureObjects::set_parameter(const GLenum pname, const GLint *params) const {
  glTexParameteriv(target(), pname, params);
}

void TextureObjects::set_parameter_I(const GLenum pname, const GLint *params) const {
#ifndef __EMSCRIPTEN__
  glTexParameterIiv(target(), pname, params);
#else
  throw std::runtime_error("glTexParameterIiv is not available in Emscripten");
#endif
}

void TextureObjects::set_parameter_I(const GLenum pname, const GLuint *params) const {
#ifndef __EMSCRIPTEN__
  glTexParameterIuiv(target(), pname, params);
#else
  throw std::runtime_error("glTexParameterIuiv is not available in Emscripten");
#endif
}

void TextureObjects::get_parameter(const GLenum pname, GLfloat *params) const {
  glGetTexParameterfv(target(), pname, params);
}

void TextureObjects::get_parameter(const GLenum pname, GLint *params) const {
  glGetTexParameteriv(target(), pname, params);
}

void TextureObjects::get_parameter_I(const GLenum pname, GLint *params) const {
#ifndef __EMSCRIPTEN__
  glGetTexParameterIiv(target(), pname, params);
#else
  throw std::runtime_error("glGetTexParameterIiv is not available in Emscripten");
#endif
}

void TextureObjects::get_parameter_I(const GLenum pname, GLuint *params) const {
#ifndef __EMSCRIPTEN__
  glGetTexParameterIuiv(target(), pname, params);
#else
  throw std::runtime_error("glGetTexParameterIuiv is not available in Emscripten");
#endif
}
} // namespace gp::gl
