#include "misc.hpp"

#include <gp/glfw/glfw.hpp>

#include <stdexcept>

namespace gp::glfw {
void gl_debug(const char *file, const int line) {
  const auto gl_error_code = glGetError();
  if (gl_error_code == GL_NO_ERROR) {
    return;
  }

  auto gl_error_code_to_string = [](const GLenum _gl_error_code) {
    if (_gl_error_code == GL_INVALID_ENUM) {
      return "GL_INVALID_ENUM";
    }
    if (_gl_error_code == GL_INVALID_VALUE) {
      return "GL_INVALID_VALUE";
    }
    if (_gl_error_code == GL_INVALID_OPERATION) {
      return "GL_INVALID_OPERATION";
    }
    if (_gl_error_code == GL_STACK_OVERFLOW) {
      return "GL_STACK_OVERFLOW";
    }
    if (_gl_error_code == GL_STACK_UNDERFLOW) {
      return "GL_STACK_UNDERFLOW";
    }
    if (_gl_error_code == GL_OUT_OF_MEMORY) {
      return "GL_OUT_OF_MEMORY";
    }
    if (_gl_error_code == GL_INVALID_FRAMEBUFFER_OPERATION) {
      return "GL_INVALID_FRAMEBUFFER_OPERATION";
    }
    return "";
  };

  printf("[OpenGL Error] %s:%i {%s}\n", file, line, gl_error_code_to_string(gl_error_code));
  throw std::runtime_error("OpenGL Error");
}
} // namespace gp::glfw
