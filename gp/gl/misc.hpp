#pragma once

#include <gp/gl/shader_program.hpp>

#include <memory>
#include <string>

namespace gp::gl {
/**
 * @brief Creates a shader program by loading vertex and fragment shader code from a file.
 *
 * This function creates a shader program by loading vertex and fragment shader code from
 * files indicated by the program_name parameter. The program_name parameter is a quasi filepath
 * without extensions (without .vs and .fs), as these extensions are added later to open the exact files.
 *
 * @param program_name The name of the shader program without extensions (e.g., "shader_program").
 * @return A unique pointer to the created shader program.
 * @throw std::runtime_error In case of errors during compilation or linking of the shader program.
 */
std::unique_ptr<ShaderProgram> create_shader_program(const std::string &program_name);
} // namespace gp::gl
