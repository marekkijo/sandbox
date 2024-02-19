#pragma once

#include <cstddef>
#include <cstdint>

#include "streaming_common/opengl.hpp"
#include <gp/common/user_input.hpp>

class GLScene {
public:
  GLScene() = delete;
  GLScene(std::size_t width, std::size_t height);
  GLScene(const GLScene &) = delete;
  GLScene &operator=(const GLScene &) = delete;
  GLScene(GLScene &&) noexcept = delete;
  GLScene &operator=(GLScene &&) noexcept = delete;
  virtual ~GLScene() = default;

  [[nodiscard]] std::size_t window_width() const;
  [[nodiscard]] std::size_t window_height() const;
  void set_window_size(std::size_t width, std::size_t height);

  [[nodiscard]] GLFWwindow *glfw_window();
  [[nodiscard]] const GLFWwindow *glfw_window() const;
  void set_glfw_window(GLFWwindow *glfw_window);

  virtual void init() = 0;
  virtual void draw() = 0;
  virtual void process_user_input(const gp::common::UserInput2 &user_input);

  static constexpr float dpi_scale_factor{1.0f};

private:
  std::size_t width_{1280UL};
  std::size_t height_{720UL};
  GLFWwindow *glfw_window_ = nullptr;
};