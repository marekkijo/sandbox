#include "encode.hpp"

#include "rgb_cube_gl_scene.hpp"

#include "streaming_common/common.hpp"
#include "streaming_common/encoder.hpp"

#include <gp/glfw/glfw.hpp>

#include <fstream>
#include <memory>
#include <optional>
#include <utility>

namespace streaming {
namespace {
struct RenderingContext {
  int width{};
  int height{};
  GLFWwindow *glfw_window{};

  //  using GLSceneType = NoisyTrianglesGLScene;
  using GLSceneType = RGBCubeGLScene;
  std::optional<GLSceneType> gl_scene;
};

void init_rendering(RenderingContext &rendering_context) {
  rendering_context.glfw_window = gp::glfw::init(rendering_context.width, rendering_context.height, "OpenGL Encoder");

  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) { printf("Couldn't initialize GLAD\n"); }

  rendering_context.gl_scene.emplace(rendering_context.width, rendering_context.height);
  rendering_context.gl_scene->set_glfw_window(rendering_context.glfw_window);
  rendering_context.gl_scene->init();
}
} // namespace

void encode(const VideoStreamInfo &video_stream_info, const int length_s) {
  printf("Encoding...\n");

  RenderingContext rendering_context{};
  rendering_context.width = video_stream_info.width;
  rendering_context.height = video_stream_info.height;

  init_rendering(rendering_context);
  if (!rendering_context.gl_scene.has_value()) {
    printf("GL scene not initialized.\n");
    return;
  }

  auto file = std::ofstream("file.h264", std::ios::out | std::ios::binary);
  auto encoder = Encoder();
  encoder.open_stream(video_stream_info);

  auto video_stream_function = std::function<void(const std::byte *data, const std::size_t size, const bool eof)>{
      [&file](const std::byte *data, const std::size_t size, const bool eof) {
        file.write(reinterpret_cast<const char *>(data), static_cast<std::streamsize>(size));
        if (eof) { printf("Encoding: end of stream\n"); }
      }};
  encoder.set_video_stream_callback(video_stream_function);

  auto video_frame = encoder.video_frame();

  const auto number_of_frames = length_s * video_stream_info.fps;
  auto frame_num = 1;

  while (glfwWindowShouldClose(rendering_context.glfw_window) == 0) {
    rendering_context.gl_scene->draw();

    constexpr auto format = CHANNELS_NUM == 4u ? GL_RGBA : GL_RGB;
    glReadPixels(0,
                 0,
                 rendering_context.width,
                 rendering_context.height,
                 format,
                 GL_UNSIGNED_BYTE,
                 video_frame->data());
    encoder.encode();

    glfwSwapBuffers(rendering_context.glfw_window);
    glfwPollEvents();

    printf("Encoded frame: %i\n", frame_num);

    if (frame_num++ == number_of_frames) { glfwSetWindowShouldClose(rendering_context.glfw_window, true); }
  }

  encoder.close_stream();

  rendering_context.gl_scene.reset();
  gp::glfw::terminate(rendering_context.glfw_window);
  rendering_context.glfw_window = nullptr;
}
} // namespace streaming
