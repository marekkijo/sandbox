#pragma once

#include <gp/glfw/scene_gl_fwd.hpp>

#include <gp/glfw/internal/glfw_context_fwd.hpp>
#include <gp/glfw/internal/glfw_window_fwd.hpp>
#include <gp/glfw/internal/user_input_state.hpp>
#include <gp/misc/event_fwd.hpp>

#include <condition_variable>
#include <memory>
#include <mutex>
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

  /**
   * Initializes the scene with the specified width, height, title of a window, and optional async flag should be set to
   * true if this call comes from a different thread than the main thread.
   *
   * @param width The width of the scene.
   * @param height The height of the scene.
   * @param title The title of the scene.
   * @param async Flag indicating if the call comes from a different thread than the main thread.
   */
  void init(const int width, const int height, const std::string &title, const bool async = false);

  /**
   * Executes the scene.
   *
   * @param async_init Flag indicating if the call should wait for the scene to be initialized (from a different
   * thread). Otherwise it is assumed that the scene is already initialized.
   * @return The exit code of the scene.
   */
  int exec(const bool async_init = false);

  virtual void loop(const misc::Event &event) = 0;

  int width() const;
  int height() const;

  std::uint32_t timestamp() const;

  void swap_buffers();
  void request_close();

private:
  void framebuffer_size_callback(const int width, const int height);
  void mouse_button_callback(const int button, const int action, const int mods);
  void cursor_pos_callback(const double xpos, const double ypos);
  void scroll_callback(const double xoffset, const double yoffset);
  void key_callback(const int key, const int scancode, const int action, const int mods);

  int width_{};
  int height_{};
  std::string title_{};

  std::shared_ptr<internal::GLFWContext> ctx_;
  std::unique_ptr<internal::GLFWWindow> wnd_;

  internal::UserInputState user_input_state_;

  std::mutex init_mutex_{};
  std::condition_variable init_cv_{};
  bool init_done_{};
};
} // namespace gp::glfw
