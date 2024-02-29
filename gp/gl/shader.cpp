#include "shader.hpp"

#include <stdexcept>

namespace gp::gl {
Shader::Shader(const GLenum type) {
  id_ = glCreateShader(type);
  if (id() == 0) { throw std::runtime_error("Failed to create shader"); }
}

Shader::Shader(const GLenum type, const std::string &code, const bool do_not_compile)
    : Shader(type) {
  source(code, do_not_compile);
}

Shader::~Shader() { glDeleteShader(id()); }

GLuint Shader::id() const { return id_; }

void Shader::source(const std::string &code, const bool do_not_compile) const {
  const char *c_code = code.c_str();
  glShaderSource(id(), 1, &c_code, nullptr);
  if (!do_not_compile) { compile(); }
}

void Shader::compile() const {
  glCompileShader(id());
  check_compile_status();
}

void Shader::check_compile_status() const {
  auto success = GL_FALSE;
  glGetShaderiv(id(), GL_COMPILE_STATUS, &success);
  if (!success) {
    auto info_log_length = 0;
    glGetShaderiv(id(), GL_INFO_LOG_LENGTH, &info_log_length);
    auto error_message = std::string(info_log_length + 1, '\0');
    glGetShaderInfoLog(id(), info_log_length, nullptr, error_message.data());
    throw std::runtime_error("Shader error: " + error_message);
  }
}
} // namespace gp::gl
