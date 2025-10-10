#pragma once

#include <gp/gl/gl.hpp>

#include <glm/glm.hpp>

#include <string>
#include <unordered_map>

namespace gp::gl {
/**
 * @brief Represents a shader program in OpenGL.
 *
 * The ShaderProgram class provides functionality to create, manage, and use shader programs in OpenGL.
 * It allows attaching shaders, linking the program, setting uniform values, and using the program for rendering.
 */
class ShaderProgram {
public:
  /**
   * @brief Default constructor.
   */
  ShaderProgram();

  /**
   * @brief Destructor.
   */
  ~ShaderProgram();

  /**
   * @brief Get the ID of the shader program.
   * @return The ID of the shader program.
   */
  GLuint id() const;

  /**
   * @brief Use the shader program for rendering.
   */
  void use() const;

  /**
   * @brief Attach a shader to the shader program.
   * @param shader The ID of the shader to attach.
   */
  void attach_shader(const GLuint shader) const;

  /**
   * @brief Detach a shader from the shader program.
   * @param shader The ID of the shader to detach.
   */
  void detach_shader(const GLuint shader) const;

  /**
   * @brief Link the shader program.
   */
  void link() const;

  /**
   * @brief Set an integer uniform value in the shader program.
   * @param name The name of the uniform variable.
   * @param value The value to set.
   */
  void set_uniform(const std::string &name, const GLint value);

  /**
   * @brief Set a floating-point uniform value in the shader program.
   * @param name The name of the uniform variable.
   * @param value The value to set.
   */
  void set_uniform(const std::string &name, const GLfloat value);

  /**
   * @brief Set a vector uniform value in the shader program.
   * @param name The name of the uniform variable.
   * @param value The value to set.
   */
  void set_uniform(const std::string &name, const glm::vec3 &value);

  /**
   * @brief Set a vector uniform value in the shader program.
   * @param name The name of the uniform variable.
   * @param value The value to set.
   */
  void set_uniform(const std::string &name, const glm::vec4 &value);

  /**
   * @brief Set a matrix uniform value in the shader program.
   * @param name The name of the uniform variable.
   * @param value The value to set.
   */
  void set_uniform(const std::string &name, const glm::mat4 &value);

private:
  /**
   * @brief Check the link status of the shader program.
   */
  void check_link_status() const;

  /**
   * @brief Get the location of a uniform variable in the shader program.
   * @param name The name of the uniform variable.
   * @return The location of the uniform variable.
   */
  GLint uniform_location(const std::string &name);

  /**
   * @brief Check the location of a uniform variable in the shader program.
   * @param name The name of the uniform variable.
   */
  void check_uniform_location(const std::string &name);

  GLuint id_{};
  std::unordered_map<std::string, GLint> uniform_locations_{};
};
} // namespace gp::gl
