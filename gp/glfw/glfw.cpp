#include "glfw.hpp"

#include <stdexcept>

namespace gp::glfw {
namespace {
constexpr auto REQUESTED_CLIENT_API = GLFW_OPENGL_API;
constexpr auto REQUESTED_CONTEXT_CREATION_API = GLFW_NATIVE_CONTEXT_API;
constexpr auto REQUESTED_GL_PROFILE = GLFW_OPENGL_CORE_PROFILE;
constexpr auto REQUESTED_GL_VERSION_MAJOR = 4;
constexpr auto REQUESTED_GL_VERSION_MINOR = 3;
} // namespace

GLFWwindow *init(const int width, const int height, const std::string &wnd_title) {
  if (glfwInit() == GLFW_FALSE) { printf("Couldn't initialize GLFW\n"); }

  glfwWindowHint(GLFW_CLIENT_API, REQUESTED_CLIENT_API);
  glfwWindowHint(GLFW_CONTEXT_CREATION_API, REQUESTED_CONTEXT_CREATION_API);
  glfwWindowHint(GLFW_OPENGL_PROFILE, REQUESTED_GL_PROFILE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, REQUESTED_GL_VERSION_MAJOR);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, REQUESTED_GL_VERSION_MINOR);

  glfwWindowHint(GLFW_DOUBLEBUFFER, 1);
  glfwWindowHint(GLFW_DEPTH_BITS, 24);

  auto *glfw_window = glfwCreateWindow(width, height, wnd_title.c_str(), nullptr, nullptr);
  if (glfw_window == nullptr) {
    printf("Couldn't create GLFW window\n");
  } else {
    glfwMakeContextCurrent(glfw_window);
  }
  return glfw_window;
}

void terminate(GLFWwindow *glfw_window) {
  if (glfw_window != nullptr) { glfwDestroyWindow(glfw_window); }
  glfwTerminate();
}

void debug(const char *file, const int line) {
  const auto gl_error_code = glGetError();
  if (gl_error_code == GL_NO_ERROR) { return; }

  auto gl_error_code_to_string = [](const GLenum _gl_error_code) {
    if (_gl_error_code == GL_INVALID_ENUM) { return "GL_INVALID_ENUM"; }
    if (_gl_error_code == GL_INVALID_VALUE) { return "GL_INVALID_VALUE"; }
    if (_gl_error_code == GL_INVALID_OPERATION) { return "GL_INVALID_OPERATION"; }
    if (_gl_error_code == GL_STACK_OVERFLOW) { return "GL_STACK_OVERFLOW"; }
    if (_gl_error_code == GL_STACK_UNDERFLOW) { return "GL_STACK_UNDERFLOW"; }
    if (_gl_error_code == GL_OUT_OF_MEMORY) { return "GL_OUT_OF_MEMORY"; }
    if (_gl_error_code == GL_INVALID_FRAMEBUFFER_OPERATION) { return "GL_INVALID_FRAMEBUFFER_OPERATION"; }
    return "";
  };

  printf("[OpenGL Error] %s:%i {%s}\n", file, line, gl_error_code_to_string(gl_error_code));
  throw std::runtime_error("OpenGL Error");
}
} // namespace gp::glfw
