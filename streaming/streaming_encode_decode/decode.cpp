#include "decode.hpp"

#include <chrono>
#include <fstream>
#include <optional>
#include <thread>

#include "fullscreen_texture_gl_scene.hpp"
#include "streaming_common/common.hpp"
#include "streaming_common/decoder.hpp"
#include "streaming_common/opengl.hpp"
#include "streaming_common/utils.hpp"

namespace streaming {
namespace {
struct RenderingContext {
  int width{};
  int height{};
  GLFWwindow *glfw_window{};
  std::optional<FullscreenTextureGLScene> gl_scene;
};

void configure_texture(RenderingContext &rendering_context) {
  GLuint texture_id;
  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glBindTexture(GL_TEXTURE_2D, 0);

  constexpr bool pass_ownership{true};
  rendering_context.gl_scene->set_texture(texture_id, pass_ownership);
}

void init_rendering(RenderingContext &rendering_context) {
  rendering_context.glfw_window = initialize_glfw(rendering_context.width, rendering_context.height, "OpenGL Decoder");

  if (gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)) == 0) {
    printf("Couldn't initialize GLAD\n");
  }

  rendering_context.gl_scene.emplace(rendering_context.width, rendering_context.height);
  rendering_context.gl_scene->set_glfw_window(rendering_context.glfw_window);
  rendering_context.gl_scene->init();

  configure_texture(rendering_context);
}

void read_some(std::ifstream &file, Decoder &decoder) {
  constexpr std::size_t buffer_size_1_kB = 1024u;

  if (file.eof()) { return; }

  static std::array<std::byte, buffer_size_1_kB> buffer{};
  file.read(reinterpret_cast<char *>(buffer.data()), buffer.size());

  const auto size = static_cast<std::size_t>(file.gcount());
  std::ignore = decoder.incoming_data(buffer.data(), size);

  if (file.eof()) { decoder.signal_eof(); }
}
} // namespace

void decode(const VideoStreamInfo &video_stream_info) {
  printf("Decoding...\n");

  RenderingContext rendering_context{};
  rendering_context.width = video_stream_info.width;
  rendering_context.height = video_stream_info.height;

  init_rendering(rendering_context);
  if (!rendering_context.gl_scene.has_value()) {
    printf("GL scene not initialized.\n");
    return;
  }

  const auto frames_interval = std::chrono::milliseconds{1000} / video_stream_info.fps;
  auto file = std::ifstream("file.h264", std::ios::in | std::ios::binary);

  auto decoder = Decoder();
  decoder.set_video_stream_info(video_stream_info);

  auto rgb_frame = decoder.rgb_frame();
  constexpr auto format = CHANNELS_NUM == 4u ? GL_RGBA : GL_RGB;

  while (glfwWindowShouldClose(rendering_context.glfw_window) == 0) {
    glfwPollEvents();
    const auto next_frame_time = std::chrono::steady_clock::now() + frames_interval;
    const auto status = decoder.decode();
    switch (status.code) {
    case Decoder::Status::Code::OK:
      printf("Decoded frame: %i\n", status.frame_num);

      glBindTexture(GL_TEXTURE_2D, rendering_context.gl_scene->texture_id());
      glTexImage2D(GL_TEXTURE_2D,
                   0,
                   format,
                   rendering_context.width,
                   rendering_context.height,
                   0,
                   format,
                   GL_UNSIGNED_BYTE,
                   rgb_frame->data());
      glBindTexture(GL_TEXTURE_2D, 0);

      rendering_context.gl_scene->draw();

      glfwSwapBuffers(rendering_context.glfw_window);

      if (next_frame_time > std::chrono::steady_clock::now()) { std::this_thread::sleep_until(next_frame_time); }
      break;
    case Decoder::Status::Code::RETRY:
      printf("Decoding: packet sent, retrying...\n");
      continue;
      break;
    case Decoder::Status::Code::EOS:
      printf("Decoding: end of stream\n");
      glfwSetWindowShouldClose(rendering_context.glfw_window, true);
      break;
    case Decoder::Status::Code::NODATA:
      printf("Decoding: no data left to decode\n");
      read_some(file, decoder);
      break;
    case Decoder::Status::Code::ERROR:
      printf("Decoding: ERROR\n");
      glfwSetWindowShouldClose(rendering_context.glfw_window, true);
      break;
    }
  }

  rendering_context.gl_scene.reset();
  terminate_glfw(rendering_context.glfw_window);
  rendering_context.glfw_window = nullptr;
}
} // namespace streaming
