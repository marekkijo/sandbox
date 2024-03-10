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

void SceneGL::init(const int width, const int height, const std::string &title) {
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

  misc::Event event(misc::Event::Type::Init, ctx_->timestamp());
  event.init().width = width;
  event.init().height = height;
  loop(event);
}

SceneGL::~SceneGL() = default;

int SceneGL::exec() {
  int close_flag;
  while ((close_flag = wnd_->window_should_close()) == 0) {
    misc::Event event(misc::Event::Type::Redraw, ctx_->timestamp());
    loop(event);
    wnd_->swap_buffers();
    glfwPollEvents();
  }

  misc::Event event(misc::Event::Type::Quit, ctx_->timestamp());
  event.quit().close_flag = close_flag;
  loop(event);

  return close_flag;
}

int SceneGL::width() const { return width_; }

int SceneGL::height() const { return height_; }

std::shared_ptr<internal::GLFWContext> SceneGL::ctx() { return ctx_; }

void SceneGL::framebuffer_size_callback(const int width, const int height) {
  misc::Event event(misc::Event::Type::Resize, ctx_->timestamp());
  event.resize().width = width;
  event.resize().height = height;
  loop(event);
}

void SceneGL::mouse_button_callback(const int button, const int action, const int mods) {
  misc::Event event(misc::Event::Type::MouseButton, ctx_->timestamp());
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
  misc::Event event(misc::Event::Type::MouseMove, ctx_->timestamp());
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
  misc::Event event(misc::Event::Type::MouseScroll, ctx_->timestamp());
  event.mouse_scroll().vertical = yoffset;
  event.mouse_scroll().horizontal = xoffset;
  loop(event);
}

void SceneGL::key_callback(const int key, const int scancode, const int action, const int mods) {
  misc::Event event(misc::Event::Type::Key, ctx_->timestamp());
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
