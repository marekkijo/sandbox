#pragma once

namespace gp::glfw::internal {
class GLFWContext {
public:
  GLFWContext();
  ~GLFWContext();

  GLFWContext(GLFWContext &&) = delete;
  GLFWContext &operator=(GLFWContext &&) = delete;
  GLFWContext(const GLFWContext &) = delete;
  GLFWContext &operator=(const GLFWContext &) = delete;

private:
  static bool context_created;
};
} // namespace gp::glfw::internal
