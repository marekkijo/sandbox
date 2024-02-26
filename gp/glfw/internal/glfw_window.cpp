#include "glfw_window.hpp"

#include <gp/glfw/glfw.hpp>

#include <stdexcept>

namespace gp::glfw::internal {
class GLFWWindow::Impl {
public:
  Impl(std::shared_ptr<GLFWContext> ctx, const int width, const int height, const std::string &title)
      : ctx_{ctx}
      , wnd_{glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr)} {
    if (!wnd_) { throw std::runtime_error{"Couldn't create GLFW window"}; }
    make_context_current();
    if (gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)) == 0) {
      throw std::runtime_error{"Couldn't initialize GLAD"};
    }

    set_window_user_pointer(this);
  }

  ~Impl() {
    glfwDestroyWindow(wnd_);
    wnd_ = nullptr;
  }

  Impl(Impl &&) = delete;
  Impl &operator=(Impl &&) = delete;
  Impl(const Impl &) = delete;
  Impl &operator=(const Impl &) = delete;

  void make_context_current() const { glfwMakeContextCurrent(wnd_); }

  void set_framebuffer_size_callback(std::function<void(const int width, const int height)> callback) {
    set_framebuffer_size_callback(loose_framebuffer_size_callback);
    framebuffer_size_callback_ = callback;
  }

  void set_mouse_button_callback(std::function<void(const int button, const int action, const int mods)> callback) {
    set_mouse_button_callback(loose_mouse_button_callback);
    mouse_button_callback_ = callback;
  }

  void set_cursor_pos_callback(std::function<void(const double xpos, const double ypos)> callback) {
    set_cursor_pos_callback(loose_cursor_pos_callback);
    cursor_pos_callback_ = callback;
  }

  void set_scroll_callback(std::function<void(const double xoffset, const double yoffset)> callback) {
    set_scroll_callback(loose_scroll_callback);
    scroll_callback_ = callback;
  }

  void
  set_key_callback(std::function<void(const int key, const int scancode, const int action, const int mods)> callback) {
    set_key_callback(loose_key_callback);
    key_callback_ = callback;
  }

  int window_should_close() const { return glfwWindowShouldClose(wnd_); }

  void swap_buffers() const { glfwSwapBuffers(wnd_); }

  int get_mouse_button(int button) const { return glfwGetMouseButton(wnd_, button); }

private:
  void set_window_user_pointer(void *ptr) const { glfwSetWindowUserPointer(wnd_, ptr); }

  GLFWframebuffersizefun set_framebuffer_size_callback(GLFWframebuffersizefun callback) const {
    return glfwSetFramebufferSizeCallback(wnd_, callback);
  }

  GLFWmousebuttonfun set_mouse_button_callback(GLFWmousebuttonfun callback) const {
    return glfwSetMouseButtonCallback(wnd_, callback);
  }

  GLFWcursorposfun set_cursor_pos_callback(GLFWcursorposfun callback) const {
    return glfwSetCursorPosCallback(wnd_, callback);
  }

  GLFWscrollfun set_scroll_callback(GLFWscrollfun callback) const { return glfwSetScrollCallback(wnd_, callback); }

  GLFWkeyfun set_key_callback(GLFWkeyfun callback) const { return glfwSetKeyCallback(wnd_, callback); }

  static void loose_framebuffer_size_callback(GLFWwindow *wnd_, const int width, const int height) {
    const auto *wnd = reinterpret_cast<Impl *>(glfwGetWindowUserPointer(wnd_));
    if (wnd->framebuffer_size_callback_) { wnd->framebuffer_size_callback_(width, height); }
  }

  static void loose_mouse_button_callback(GLFWwindow *wnd_, const int button, const int action, const int mods) {
    const auto *wnd = reinterpret_cast<Impl *>(glfwGetWindowUserPointer(wnd_));
    if (wnd->mouse_button_callback_) { wnd->mouse_button_callback_(button, action, mods); }
  }

  static void loose_cursor_pos_callback(GLFWwindow *wnd_, const double xpos, const double ypos) {
    const auto *wnd = reinterpret_cast<Impl *>(glfwGetWindowUserPointer(wnd_));
    if (wnd->cursor_pos_callback_) { wnd->cursor_pos_callback_(xpos, ypos); }
  }

  static void loose_scroll_callback(GLFWwindow *wnd_, const double xoffset, const double yoffset) {
    const auto *wnd = reinterpret_cast<Impl *>(glfwGetWindowUserPointer(wnd_));
    if (wnd->scroll_callback_) { wnd->scroll_callback_(xoffset, yoffset); }
  }

  static void
  loose_key_callback(GLFWwindow *wnd_, const int key, const int scancode, const int action, const int mods) {
    const auto *wnd = reinterpret_cast<Impl *>(glfwGetWindowUserPointer(wnd_));
    if (wnd->key_callback_) { wnd->key_callback_(key, scancode, action, mods); }
  }

  const std::shared_ptr<const GLFWContext> ctx_{};
  GLFWwindow *wnd_{nullptr};
  std::function<void(const int width, const int height)> framebuffer_size_callback_{};
  std::function<void(const int button, const int action, const int mods)> mouse_button_callback_{};
  std::function<void(const double xpos, const double ypos)> cursor_pos_callback_{};
  std::function<void(const double xoffset, const double yoffset)> scroll_callback_{};
  std::function<void(const int key, const int scancode, const int action, const int mods)> key_callback_{};
};

GLFWWindow::GLFWWindow(std::shared_ptr<GLFWContext> ctx, const int width, const int height, const std::string &title)
    : impl_{std::make_unique<GLFWWindow::Impl>(ctx, width, height, title)} {}

GLFWWindow::~GLFWWindow() = default;

void GLFWWindow::make_context_current() const { impl_->make_context_current(); }

void GLFWWindow::set_framebuffer_size_callback(std::function<void(const int width, const int height)> callback) {
  impl_->set_framebuffer_size_callback(callback);
}

void GLFWWindow::set_mouse_button_callback(
    std::function<void(const int button, const int action, const int mods)> callback) {
  impl_->set_mouse_button_callback(callback);
}

void GLFWWindow::set_cursor_pos_callback(std::function<void(const double xpos, const double ypos)> callback) {
  impl_->set_cursor_pos_callback(callback);
}

void GLFWWindow::set_scroll_callback(std::function<void(const double xoffset, const double yoffset)> callback) {
  impl_->set_scroll_callback(callback);
}

void GLFWWindow::set_key_callback(
    std::function<void(const int key, const int scancode, const int action, const int mods)> callback) {
  impl_->set_key_callback(callback);
}

int GLFWWindow::window_should_close() const { return impl_->window_should_close(); }

void GLFWWindow::swap_buffers() const { impl_->swap_buffers(); }

int GLFWWindow::get_mouse_button(int button) const { return impl_->get_mouse_button(button); }
} // namespace gp::glfw::internal
