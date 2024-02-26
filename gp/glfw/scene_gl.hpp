#pragma once

#include <gp/glfw/internal/glfw_context_fwd.hpp>
#include <gp/glfw/internal/glfw_window_fwd.hpp>
#include <gp/glfw/internal/user_input_state.hpp>
#include <gp/misc/event_fwd.hpp>

#include <memory>
#include <string>

namespace gp::glfw {
class SceneGL {
public:
  explicit SceneGL(std::shared_ptr<internal::GLFWContext> ctx = nullptr);
  virtual ~SceneGL();

  SceneGL(SceneGL &&) = delete;
  SceneGL &operator=(SceneGL &&) = delete;
  SceneGL(const SceneGL &) = delete;
  SceneGL &operator=(const SceneGL &) = delete;

  void init(const int width, const int height, const std::string &title);
  int exec();

  virtual void loop(const misc::Event &event) = 0;

  void set_fps(const int fps);

  int width() const;
  int height() const;

  std::shared_ptr<internal::GLFWContext> ctx();

private:
  std::uint32_t timestamp() const;
  void framebuffer_size_callback(const int width, const int height);
  void mouse_button_callback(const int button, const int action, const int mods);
  void cursor_pos_callback(const double xpos, const double ypos);
  void scroll_callback(const double xoffset, const double yoffset);
  void key_callback(const int key, const int scancode, const int action, const int mods);

  int width_{};
  int height_{};
  std::string title_{};
  int fps_{60};

  std::shared_ptr<internal::GLFWContext> ctx_;
  std::unique_ptr<internal::GLFWWindow> wnd_;

  internal::UserInputState user_input_state_;
};
} // namespace gp::glfw
