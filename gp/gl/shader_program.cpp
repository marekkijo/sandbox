#include "shader_program.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <stdexcept>

namespace gp::gl {
ShaderProgram::ShaderProgram() {
  id_ = glCreateProgram();
  if (id() == 0) { throw std::runtime_error("Failed to create shader program"); }
}

ShaderProgram::~ShaderProgram() { glDeleteProgram(id()); }

GLuint ShaderProgram::id() const { return id_; }

void ShaderProgram::use() const { glUseProgram(id()); }

void ShaderProgram::attachShader(const GLuint shader) const { glAttachShader(id(), shader); }

void ShaderProgram::detachShader(const GLuint shader) const { glDetachShader(id(), shader); }

void ShaderProgram::link() const {
  glLinkProgram(id());
  check_link_status();
}

void ShaderProgram::setUniform(const std::string &name, const GLint value) {
  glUniform1i(uniform_location(name), value);
}

void ShaderProgram::setUniform(const std::string &name, const GLfloat value) {
  glUniform1f(uniform_location(name), value);
}

void ShaderProgram::setUniform(const std::string &name, const glm::vec3 &value) {
  glUniform3fv(uniform_location(name), 1, glm::value_ptr(value));
}

void ShaderProgram::setUniform(const std::string &name, const glm::vec4 &value) {
  glUniform4fv(uniform_location(name), 1, glm::value_ptr(value));
}

void ShaderProgram::setUniform(const std::string &name, const glm::mat4 &value) {
  glUniformMatrix4fv(uniform_location(name), 1, GL_FALSE, glm::value_ptr(value));
}

void ShaderProgram::check_link_status() const {
  auto success = GL_FALSE;
  glGetProgramiv(id(), GL_LINK_STATUS, &success);
  if (!success) {
    auto info_log_length = 0;
    glGetProgramiv(id(), GL_INFO_LOG_LENGTH, &info_log_length);
    auto error_message = std::string(info_log_length + 1, '\0');
    glGetProgramInfoLog(id(), info_log_length, nullptr, error_message.data());
    throw std::runtime_error("Program error: " + error_message);
  }
}

GLint ShaderProgram::uniform_location(const std::string &name) {
  check_uniform_location(name);
  return uniform_locations_.at(name);
}

void ShaderProgram::check_uniform_location(const std::string &name) {
  if (uniform_locations_.count(name) == 0) {
    uniform_locations_[name] = glGetUniformLocation(id(), name.c_str());
    if (uniform_locations_[name] == -1) { throw std::runtime_error("Uniform " + name + " not found"); }
  }
}
} // namespace gp::gl
