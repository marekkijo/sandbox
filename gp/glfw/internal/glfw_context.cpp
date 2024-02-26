#include "glfw_context.hpp"

#include <gp/glfw/glfw.hpp>

#include <stdexcept>

namespace gp::glfw::internal {
GLFWContext::GLFWContext() {
  if (context_created) { throw std::runtime_error("GLFW already initialized"); }
  if (glfwInit() == GLFW_FALSE) { throw std::runtime_error{"Couldn't initialize GLFW"}; }
  context_created = true;
}

GLFWContext::~GLFWContext() {
  glfwTerminate();
  context_created = false;
}

bool GLFWContext::context_created{false};
} // namespace gp::glfw::internal
