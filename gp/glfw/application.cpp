#include "gp/glfw/application.hpp"

namespace gp::glfw {
Application::Application(Args args, AppConfig app_config, GlConfig gl_config) {}

Application::AppConfig &Application::AppConfig::set_title(std::string title_) {
  title = std::move(title_);
  return *this;
}

Application::AppConfig &Application::AppConfig::set_size(const glm::uvec2 &size_) {
  size = size_;
  return *this;
}

Application::AppConfig &Application::AppConfig::set_resizable(const bool resizable_) {
  resizable = resizable_;
  if (resizable) {
    fullscreen = false;
  }
  return *this;
}

Application::AppConfig &Application::AppConfig::set_fullscreen(const bool fullscreen_) {
  fullscreen = fullscreen_;
  if (fullscreen) {
    resizable = false;
  }
  return *this;
}
} // namespace gp::glfw
