#pragma once

#include <gp/glfw/internal/glfw_context_fwd.hpp>

#include <cstdint>

namespace gp::glfw::internal {
class GLFWContext {
public:
  GLFWContext();
  ~GLFWContext();

  GLFWContext(GLFWContext &&) = delete;
  GLFWContext &operator=(GLFWContext &&) = delete;
  GLFWContext(const GLFWContext &) = delete;
  GLFWContext &operator=(const GLFWContext &) = delete;

  /**
   * Returns the timestamp of the GLFW context.
   *
   * @return The number of milliseconds since the context was created.
   */
  std::uint32_t timestamp() const;

private:
  static bool context_created_;
};
} // namespace gp::glfw::internal
