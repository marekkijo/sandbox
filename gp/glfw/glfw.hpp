#pragma once

#include <string>

/* clang-format off */

#include <glad/glad.h> // must be included before GLFW
#include <GLFW/glfw3.h>

/* clang-format on */

namespace gp::glfw {
GLFWwindow *init(const int width, const int height, const std::string &wnd_title);
void terminate(GLFWwindow *glfw_window);
void debug(const char *file, const int line);
} // namespace gp::glfw
