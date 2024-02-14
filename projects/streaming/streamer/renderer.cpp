#include <GL/glew.h>

#include "renderer.hpp"

#include "common/common.hpp"

#include "tools/utils/utils.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SDL2/SDL.h>

#include <functional>

namespace streaming {
Renderer::Renderer(int width, int height, std::uint16_t fps, std::shared_ptr<Encoder> &encoder)
    : width_{width}
    , height_{height}
    , fps_{fps}
    , encoder_{encoder}
    , gl_frame_{encoder_->gl_frame()} {
  start_render_thread();
}

Renderer::~Renderer() {
  quit_ = true;
  render_thread_.join();
}

void Renderer::render_procedure() {
  init_rendering();

  auto last_timestamp_ms = SDL_GetTicks();

  while (!quit_) {
    SDL_Event event;
    while (!quit_ && SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
        quit_ = true;
        break;
      case SDL_USEREVENT:
        SDL_PumpEvents();
        {
          glUseProgram(shader_program_);
          const auto time_elapsed_ms = event.user.timestamp - last_timestamp_ms;
          animate(time_elapsed_ms);
          last_timestamp_ms = event.user.timestamp;

          glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

          auto camera_rot_mat = glm::rotate(glm::mat4(1.0f), glm::radians(camera_rot_.x), glm::vec3(1.0f, 0.0f, 0.0f));
          camera_rot_mat = glm::rotate(camera_rot_mat, glm::radians(camera_rot_.y), glm::vec3(0.0f, 1.0f, 0.0f));
          camera_rot_mat = glm::rotate(camera_rot_mat, glm::radians(camera_rot_.z), glm::vec3(0.0f, 0.0f, 1.0f));
          glUniformMatrix4fv(camera_rot_location_, 1, GL_FALSE, glm::value_ptr(camera_rot_mat));

          const auto view = glm::lookAt(camera_pos_, glm::vec3(0, 0, 0), glm::vec3(0, 0, 1));
          const auto viewport_mat = projection_ * view;
          glUniformMatrix4fv(viewport_location_, 1, GL_FALSE, glm::value_ptr(viewport_mat));

          glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

          glFlush();
          constexpr auto format = CHANNELS_NUM == 4u ? GL_RGBA : GL_RGB;
          glReadPixels(0, 0, width_, height_, format, GL_UNSIGNED_BYTE, gl_frame_->data());
          encoder_->encode_frame();

          SDL_GL_SwapWindow(sdl_sys_->wnd());
        }
        break;
      default:
        break;
      }
    }
  }

  encoder_->close_stream();

  animation_.reset();
  sdl_sys_.reset();
}

void Renderer::init_rendering() {
  sdl_sys_ = std::make_unique<tools::sdl::SDLSystem>(SDL_INIT_EVERYTHING,
                                                     STREAMER_ID,
                                                     SDL_WINDOWPOS_CENTERED,
                                                     SDL_WINDOWPOS_CENTERED,
                                                     width_,
                                                     height_,
                                                     SDL_WINDOW_OPENGL,
                                                     -1,
                                                     SDL_RENDERER_ACCELERATED);
  animation_ = std::make_unique<tools::sdl::SDLAnimation>(fps_);

  glewInit();

  glEnable(GL_DEPTH_TEST);
  glFrontFace(GL_CW);
  glEnable(GL_CULL_FACE);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glViewport(0, 0, width_, height_);

  glReadBuffer(GL_BACK);

  projection_ = glm::perspective(glm::radians(60.0f), static_cast<float>(width_) / height_, 1.0f, 1024.0f);
  camera_pos_ = glm::vec3{4.0f, 4.0f, 0.0f};
  camera_rot_ = glm::vec3{45.0f, 45.0f, 45.0f};

  shader_program_ = load_shader_program("projects/streaming/shader_program");
  configure_program();
}

void Renderer::animate(Uint32 time_elapsed_ms) {
  const float speed_factor = static_cast<float>(time_elapsed_ms) / 100.0f;

  camera_rot_ += speed_factor;
}

void Renderer::start_render_thread() {
  auto render_procedure = std::function<void()>{std::bind(&Renderer::render_procedure, this)};
  render_thread_ = std::thread(render_procedure);
}

void Renderer::configure_program() {
  static GLuint indices_data[] = {
      0, 1, 2, 2, 3, 0, 0, 3, 4, 4, 3, 5, 5, 6, 4, 6, 7, 4, 4, 7, 0, 0, 7, 1, 1, 7, 6, 6, 2, 1, 3, 2, 6, 6, 5, 3,
  };
  GLuint indices_buf;
  glGenBuffers(1, &indices_buf);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_buf);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_data), indices_data, GL_STATIC_DRAW);

  const GLfloat vertex_data[] = {
      -1, -1, -1, 1, +1, -1, -1, 1, +1, +1, -1, 1, -1, +1, -1, 1,
      -1, -1, +1, 1, -1, +1, +1, 1, +1, +1, +1, 1, +1, -1, +1, 1,
  };
  GLuint vertex_buf;
  glGenBuffers(1, &vertex_buf);
  glEnableVertexAttribArray(glGetAttribLocation(shader_program_, "vertex_buf"));
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buf);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);
  glVertexAttribPointer(glGetAttribLocation(shader_program_, "vertex_buf"), 4, GL_FLOAT, GL_FALSE, 0, 0);

  const GLfloat color_data[] = {
      1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 0, 1, 0, 1, 1, 1,
  };
  GLuint color_buf;
  glGenBuffers(1, &color_buf);
  glEnableVertexAttribArray(glGetAttribLocation(shader_program_, "color_buf"));
  glBindBuffer(GL_ARRAY_BUFFER, color_buf);
  glBufferData(GL_ARRAY_BUFFER, sizeof(color_data), color_data, GL_STATIC_DRAW);
  glVertexAttribPointer(glGetAttribLocation(shader_program_, "color_buf"), 4, GL_FLOAT, GL_FALSE, 0, 0);

  viewport_location_ = glGetUniformLocation(shader_program_, "viewport");
  camera_rot_location_ = glGetUniformLocation(shader_program_, "camera_rot");
}

GLuint Renderer::load_shader_program(const std::string &program_name) {
  const auto vert_shader_code = tools::utils::load_txt_file(program_name + ".vs");
  const auto vert_shader_code_c_str = vert_shader_code.c_str();

  const auto vert_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vert_shader, 1, &vert_shader_code_c_str, NULL);
  glCompileShader(vert_shader);

  check_status(vert_shader, GL_COMPILE_STATUS);

  const auto frag_shader_code = tools::utils::load_txt_file(program_name + ".fs");
  const auto frag_shader_code_c_str = frag_shader_code.c_str();

  const auto frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(frag_shader, 1, &frag_shader_code_c_str, NULL);
  glCompileShader(frag_shader);

  check_status(frag_shader, GL_COMPILE_STATUS);

  const auto shader_program = glCreateProgram();

  glAttachShader(shader_program, vert_shader);
  glAttachShader(shader_program, frag_shader);
  glLinkProgram(shader_program);

  check_status(shader_program, GL_LINK_STATUS);

  glDetachShader(shader_program, vert_shader);
  glDetachShader(shader_program, frag_shader);

  glDeleteShader(vert_shader);
  glDeleteShader(frag_shader);

  return shader_program;
}

void Renderer::check_status(GLuint program, GLenum status) {
  auto result = GL_FALSE;
  auto info_log_length = 0;

  glGetShaderiv(program, status, &result);
  glGetShaderiv(program, GL_INFO_LOG_LENGTH, &info_log_length);
  if (info_log_length > 0) {
    auto error_message = std::string(info_log_length + 1, '\0');
    glGetShaderInfoLog(program, info_log_length, NULL, error_message.data());
    printf("Shader error:\n%s\n", error_message.c_str());
  }
}

} // namespace streaming
