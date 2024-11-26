#pragma once

#include <string>

namespace gp::glfw {
class Application {
public:
  struct Args;
  struct AppConfig;
  struct GlConfig;

  explicit Application(Args args, AppConfig app_config = {}, GlConfig gl_config = {});
}; // class Application

struct Application::Args {
  int argc;
  char *argv[];
}; // struct Application::Args

/// @brief Configuration for the application.
struct Application::AppConfig {
  std::string title{"GLFW Application"};
  glm::uvec2 size{800, 600};
  bool resizable{true};
  bool fullscreen{false};

  /// @brief Sets the title of the application window.
  /// @param title_ The new title of the application window.
  /// @return A reference to this object.
  AppConfig &set_title(std::string title_);

  /// @brief Sets the size of the application window.
  /// @param size_ The new size of the application window.
  /// @return A reference to this object.
  AppConfig &set_size(const glm::uvec2 &size_);

  /// @brief Sets whether the application window should be resizable.
  /// Note that if the window is resizable, it cannot be fullscreen, therefore if resizable is set to true, fullscreen
  /// is automatically set to false.
  /// @param resizable_ Whether the application window should be resizable.
  /// @return A reference to this object.
  AppConfig &set_resizable(const bool resizable_);

  /// @brief Sets whether the application window should be fullscreen.
  /// Note that if the window is fullscreen, it cannot be resizable, therefore if fullscreen is set to true, resizable
  /// is automatically set to false.
  /// @param fullscreen_ Whether the application window should be fullscreen.
  /// @return A reference to this object.
  AppConfig &set_fullscreen(const bool fullscreen_);
}; // struct Application::AppConfig

struct Application::GlConfig {
  int major_version;
  int minor_version;
  bool core_profile;
}; // struct Application::GlConfig
} // namespace gp::glfw
