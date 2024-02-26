#pragma once

#include <gp/glfw/internal/glfw_context_fwd.hpp>

#include <functional>
#include <memory>
#include <string>

namespace gp::glfw::internal {
class GLFWWindow {
public:
  GLFWWindow(std::shared_ptr<GLFWContext> ctx, const int width, const int height, const std::string &title);
  ~GLFWWindow();

  GLFWWindow(GLFWWindow &&) = delete;
  GLFWWindow &operator=(GLFWWindow &&) = delete;
  GLFWWindow(const GLFWWindow &) = delete;
  GLFWWindow &operator=(const GLFWWindow &) = delete;

  void make_context_current() const;
  void set_framebuffer_size_callback(std::function<void(const int width, const int height)> callback);
  void set_mouse_button_callback(std::function<void(const int button, const int action, const int mods)> callback);
  void set_cursor_pos_callback(std::function<void(const double xpos, const double ypos)> callback);
  void set_scroll_callback(std::function<void(const double xoffset, const double yoffset)> callback);
  void
  set_key_callback(std::function<void(const int key, const int scancode, const int action, const int mods)> callback);
  int window_should_close() const;
  void swap_buffers() const;
  int get_mouse_button(int button) const;

private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};
} // namespace gp::glfw::internal
