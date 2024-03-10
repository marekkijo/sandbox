#include "glfw_context.hpp"

#include <gp/glfw/glfw.hpp>

#include <stdexcept>

namespace gp::glfw::internal {
GLFWContext::GLFWContext() {
  if (context_created_) { throw std::runtime_error("GLFW context already created"); }
  if (glfwInit() == GLFW_FALSE) { throw std::runtime_error("GLFW initialization failed"); }
  context_created_ = true;
}

GLFWContext::~GLFWContext() {
  glfwTerminate();
  context_created_ = false;
}

std::uint32_t GLFWContext::timestamp() const { return static_cast<std::uint32_t>(glfwGetTime() * 1000.0); }

bool GLFWContext::context_created_{false};
} // namespace gp::glfw::internal
