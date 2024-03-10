#include "scene_gl.hpp"

#include <gp/glfw/glfw.hpp>
#include <gp/glfw/internal/glfw_context.hpp>
#include <gp/glfw/internal/glfw_window.hpp>
#include <gp/glfw/internal/misc.hpp>
#include <gp/misc/event.hpp>

#include <stdexcept>

namespace gp::glfw {
SceneGL::SceneGL(std::shared_ptr<internal::GLFWContext> ctx)
    : ctx_{ctx ? ctx : std::make_shared<internal::GLFWContext>()} {}

void SceneGL::init(const int width, const int height, const std::string &title, const bool async) {
  width_ = width;
  height_ = height;
  title_ = title;
  if (async) {
    const auto lock_guard = std::lock_guard(init_mutex_);
    init_done_ = true;
    init_cv_.notify_one();
    return;
  }

  if (wnd_) { return; }

  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
#ifndef NDEBUG
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
  printf("OpenGL debug context enabled\n");
#endif

  wnd_ = std::make_unique<internal::GLFWWindow>(ctx_, width, height, title);

#ifndef NDEBUG
  printf("OpenGL version: %s\n", glGetString(GL_VERSION));
#endif

  wnd_->set_framebuffer_size_callback(
      [this](const int width, const int height) { framebuffer_size_callback(width, height); });
  wnd_->set_mouse_button_callback(
      [this](const int button, const int action, const int mods) { mouse_button_callback(button, action, mods); });
  wnd_->set_cursor_pos_callback([this](const double xpos, const double ypos) { cursor_pos_callback(xpos, ypos); });
  wnd_->set_scroll_callback([this](const double xoffset, const double yoffset) { scroll_callback(xoffset, yoffset); });
  wnd_->set_key_callback([this](const int key, const int scancode, const int action, const int mods) {
    key_callback(key, scancode, action, mods);
  });
}

SceneGL::~SceneGL() = default;

int SceneGL::exec(const bool async_init) {
  if (async_init) {
    auto unique_lock = std::unique_lock(init_mutex_);
    init_cv_.wait(unique_lock, [this] { return init_done_; });
    init(width_, height_, title_);
  }

  {
    misc::Event event(misc::Event::Type::Init, timestamp());
    event.init().width = width();
    event.init().height = height();
    loop(event);
  }

  int close_flag;
  while ((close_flag = wnd_->window_should_close()) == 0) {
    misc::Event event(misc::Event::Type::Redraw, timestamp());
    loop(event);
    glfwPollEvents();
  }

  misc::Event event(misc::Event::Type::Quit, timestamp());
  event.quit().close_flag = close_flag;
  loop(event);

  wnd_.reset();

  return close_flag;
}

int SceneGL::width() const { return width_; }

int SceneGL::height() const { return height_; }

std::uint32_t SceneGL::timestamp() const { return ctx_->timestamp(); }

void SceneGL::swap_buffers() { wnd_->swap_buffers(); }

void SceneGL::request_close() { wnd_->request_close(); }

void SceneGL::framebuffer_size_callback(const int width, const int height) {
  misc::Event event(misc::Event::Type::Resize, timestamp());
  event.resize().width = width_ = width;
  event.resize().height = height_ = height;
  loop(event);
}

void SceneGL::mouse_button_callback(const int button, const int action, const int mods) {
  misc::Event event(misc::Event::Type::MouseButton, timestamp());
  switch (action) {
  case GLFW_PRESS:
    event.mouse_button().action = misc::Event::Action::Pressed;
    break;
  case GLFW_RELEASE:
    event.mouse_button().action = misc::Event::Action::Released;
    break;
  default:
    event.mouse_button().action = misc::Event::Action::None;
    break;
  }
  switch (button) {
  case GLFW_MOUSE_BUTTON_LEFT:
    event.mouse_button().button = misc::Event::MouseButton::Left;
    break;
  case GLFW_MOUSE_BUTTON_MIDDLE:
    event.mouse_button().button = misc::Event::MouseButton::Middle;
    break;
  case GLFW_MOUSE_BUTTON_RIGHT:
    event.mouse_button().button = misc::Event::MouseButton::Right;
    break;
  default:
    event.mouse_button().button = misc::Event::MouseButton::None;
    break;
  }
  loop(event);
}

void SceneGL::cursor_pos_callback(const double xpos, const double ypos) {
  misc::Event event(misc::Event::Type::MouseMove, timestamp());
  event.mouse_move().x = static_cast<float>(xpos);
  event.mouse_move().y = static_cast<float>(ypos);
  event.mouse_move().x_rel = static_cast<float>(xpos - user_input_state_.last_xpos);
  event.mouse_move().y_rel = static_cast<float>(ypos - user_input_state_.last_ypos);
  event.mouse_move().mouse_button_mask = misc::Event::MouseButtonMask::None;
  if (wnd_->get_mouse_button(GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
    event.mouse_move().mouse_button_mask |= misc::Event::MouseButtonMask::Left;
  }
  if (wnd_->get_mouse_button(GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
    event.mouse_move().mouse_button_mask |= misc::Event::MouseButtonMask::Middle;
  }
  if (wnd_->get_mouse_button(GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
    event.mouse_move().mouse_button_mask |= misc::Event::MouseButtonMask::Right;
  }
  loop(event);

  user_input_state_.last_xpos = xpos;
  user_input_state_.last_ypos = ypos;
}

void SceneGL::scroll_callback(const double xoffset, const double yoffset) {
  misc::Event event(misc::Event::Type::MouseScroll, timestamp());
  event.mouse_scroll().vertical = yoffset;
  event.mouse_scroll().horizontal = xoffset;
  loop(event);
}

void SceneGL::key_callback(const int key, const int scancode, const int action, const int mods) {
  misc::Event event(misc::Event::Type::Key, timestamp());
  switch (action) {
  case GLFW_PRESS:
    event.key().action = misc::Event::Action::Pressed;
    break;
  case GLFW_RELEASE:
    event.key().action = misc::Event::Action::Released;
    break;
  default:
    event.key().action = misc::Event::Action::None;
    break;
  }
  event.key().scan_code = internal::to_scan_code(scancode);
  loop(event);
}
} // namespace gp::glfw
