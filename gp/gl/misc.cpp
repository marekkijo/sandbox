#include "misc.hpp"

#include <gp/gl/shader.hpp>
#include <gp/utils/utils.hpp>

#include <stdexcept>

namespace gp::gl {
std::unique_ptr<ShaderProgram> create_shader_program(const std::string &program_name) {
  const auto vertex_shader_code = gp::utils::load_txt_file(program_name + ".vs");
  const auto fragment_shader_code = gp::utils::load_txt_file(program_name + ".fs");

  const auto vertex_shader = std::make_unique<Shader>(GL_VERTEX_SHADER, vertex_shader_code);
  const auto fragment_shader = std::make_unique<Shader>(GL_FRAGMENT_SHADER, fragment_shader_code);

  auto shader_program = std::make_unique<ShaderProgram>();
  shader_program->attachShader(vertex_shader->id());
  shader_program->attachShader(fragment_shader->id());
  shader_program->link();
  shader_program->detachShader(vertex_shader->id());
  shader_program->detachShader(fragment_shader->id());

  return shader_program;
}
} // namespace gp::gl
