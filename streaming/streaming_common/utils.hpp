#pragma once

#include <string>

#include "ffmpeg.hpp"
#include "opengl.hpp"

namespace streaming {
GLFWwindow *initialize_glfw(const int width, const int height, const std::string &wnd_title);
void terminate_glfw(GLFWwindow *glfw_window);
void gl_debug(const char *file, const int line);
AVCodecID codec_name_to_id(const std::string &codec_name);
} // namespace streaming
