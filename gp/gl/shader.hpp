#pragma once

#include <gp/gl/gl.hpp>

#include <string>

namespace gp::gl {
/**
 * @brief Represents a shader object in OpenGL.
 */
class Shader {
public:
  /**
   * @brief Constructs a Shader object with the specified shader type.
   * @param type The type of the shader (e.g., GL_VERTEX_SHADER, GL_FRAGMENT_SHADER).
   */
  explicit Shader(const GLenum type);

  /**
   * @brief Constructs a Shader object with the specified shader type and shader code.
   * @param type The type of the shader (e.g., GL_VERTEX_SHADER, GL_FRAGMENT_SHADER).
   * @param code The source code of the shader.
   * @param do_not_compile Flag indicating whether to compile the shader immediately after construction.
   */
  Shader(const GLenum type, const std::string &code, const bool do_not_compile = false);

  /**
   * @brief Destructor for the Shader object.
   */
  ~Shader();

  /**
   * @brief Returns the ID of the shader object.
   * @return The ID of the shader object.
   */
  GLuint id() const;

  /**
   * @brief Sets the source code of the shader.
   * @param code The new source code of the shader.
   * @param do_not_compile Flag indicating whether to compile the shader after setting the source code.
   */
  void source(const std::string &code, const bool do_not_compile = false) const;

  /**
   * @brief Compiles the shader.
   */
  void compile() const;

private:
  /**
   * @brief Checks the compile status of the shader.
   */
  void check_compile_status() const;

  GLuint id_{};
};
} // namespace gp::gl
