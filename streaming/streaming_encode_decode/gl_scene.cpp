#include "gl_scene.hpp"

GLScene::GLScene(std::size_t width, std::size_t height)
    : width_{width}
    , height_{height} {}

std::size_t GLScene::window_width() const { return width_; }

std::size_t GLScene::window_height() const { return height_; }

void GLScene::set_window_size(const std::size_t width, const std::size_t height) {
  width_ = width;
  height_ = height;
}

GLFWwindow *GLScene::glfw_window() { return glfw_window_; }

const GLFWwindow *GLScene::glfw_window() const { return glfw_window_; }

void GLScene::set_glfw_window(GLFWwindow *glfw_window) { glfw_window_ = glfw_window; }

void GLScene::process_user_input(const gp::common::UserInput2 & /*user_input*/) {}
