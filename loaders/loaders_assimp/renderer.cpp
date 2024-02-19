#include <GL/glew.h>

#include "renderer.hpp"

#include <gp/utils/utils.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SDL2/SDL.h>

#include <chrono>
#include <functional>

namespace {
void check_gl_error(const int line_num = -1) {
  auto err_to_str = [](const GLenum error_code) {
    switch (error_code) {
    case GL_INVALID_ENUM:
      return "GL_INVALID_ENUM";
    case GL_INVALID_VALUE:
      return "GL_INVALID_VALUE";
    case GL_INVALID_OPERATION:
      return "GL_INVALID_OPERATION";
    case GL_INVALID_FRAMEBUFFER_OPERATION:
      return "GL_INVALID_FRAMEBUFFER_OPERATION";
    case GL_OUT_OF_MEMORY:
      return "GL_OUT_OF_MEMORY";
    case GL_STACK_UNDERFLOW:
      return "GL_STACK_UNDERFLOW";
    case GL_STACK_OVERFLOW:
      return "GL_STACK_OVERFLOW";
    }
    return "";
  };

  const auto error_code = glGetError();

  if (error_code != GL_NO_ERROR) {
    if (line_num == -1) {
      printf("%s\n", err_to_str(error_code));
    } else {
      printf("%i: %s\n", line_num, err_to_str(error_code));
    }
  }
}
} // namespace

Renderer::Renderer(int width, int height, std::uint16_t fps, std::shared_ptr<const Model> &model)
    : width_{width}
    , height_{height}
    , fps_{fps}
    , model_{model} {
  start_render_thread();
}

Renderer::~Renderer() {
  quit_ = true;
  if (render_thread_.joinable()) { render_thread_.join(); }
}

void Renderer::start_render_thread() {
  if (render_thread_.joinable()) { return; }
  auto render_procedure = std::function<void()>{std::bind(&Renderer::render_procedure, this)};
  render_thread_ = std::thread(render_procedure);
}

void Renderer::process_user_input(const gp::common::UserInput &user_input) {
  switch (user_input.type) {
  case SDL_MOUSEMOTION:
    if (user_input.state & SDL_BUTTON_LMASK) {
      const float speed_factor = 0.1f;

      const auto lg = std::lock_guard{mutex_};
      camera_rot_.x += user_input.y_relative * speed_factor;
      camera_rot_.y += user_input.x_relative * speed_factor;
    }
    break;
  case SDL_MOUSEBUTTONDOWN: {
    const auto lg = std::lock_guard{mutex_};
    animate_ = false;
  } break;
  case SDL_MOUSEWHEEL: {
    const float speed_factor = 0.1f;
    const auto zoom_direction = user_input.y_float < 0.0f;
    const auto zoom_amount = std::fabsf(user_input.y_float) * speed_factor;

    const auto lg = std::lock_guard{mutex_};
    animate_ = false;
    if (zoom_direction) {
      camera_pos_ *= 1.0f + zoom_amount;
    } else {
      camera_pos_ *= 1.0f - zoom_amount;
    }
  } break;
  default:
    break;
  }
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

          {
            const auto lg = std::lock_guard{mutex_};
            auto camera_rot_mat =
                glm::rotate(glm::mat4(1.0f), glm::radians(camera_rot_.x), glm::vec3(1.0f, 0.0f, 0.0f));
            camera_rot_mat = glm::rotate(camera_rot_mat, glm::radians(camera_rot_.y), glm::vec3(0.0f, 1.0f, 0.0f));
            camera_rot_mat = glm::rotate(camera_rot_mat, glm::radians(camera_rot_.z), glm::vec3(0.0f, 0.0f, 1.0f));
            glUniformMatrix4fv(camera_rot_location_, 1, GL_FALSE, glm::value_ptr(camera_rot_mat));
          }

          const auto view = glm::lookAt(camera_pos_, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
          const auto viewport_mat = projection_ * view;
          glUniformMatrix4fv(viewport_location_, 1, GL_FALSE, glm::value_ptr(viewport_mat));

          const auto vertices_buf_location = glGetAttribLocation(shader_program_, "vertices_buf");
          for (std::size_t i = 0; i < indices_bufs_.size(); i++) {
            glBindBuffer(GL_ARRAY_BUFFER, vertices_bufs_[i]);
            glVertexAttribPointer(vertices_buf_location, 4, GL_FLOAT, GL_FALSE, 0, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_bufs_[i]);
            glUniform4fv(color_location_, 1, glm::value_ptr(colors_[i]));
            glDrawElements(GL_TRIANGLES, sizes_[i], GL_UNSIGNED_INT, 0);
          }

          SDL_GL_SwapWindow(sdl_sys_->wnd());
        }
        break;
      case SDL_MOUSEMOTION: {
        auto user_input = gp::common::UserInput{event.motion.type, event.motion.timestamp};
        user_input.state = event.motion.state;
        user_input.x = event.motion.x;
        user_input.y = event.motion.y;
        user_input.x_relative = event.motion.xrel;
        user_input.y_relative = event.motion.yrel;
        process_user_input(user_input);
      } break;
      case SDL_MOUSEBUTTONDOWN:
      case SDL_MOUSEBUTTONUP: {
        auto user_input = gp::common::UserInput{event.button.type, event.button.timestamp};
        user_input.state = event.button.state;
        user_input.x = event.button.x;
        user_input.y = event.button.y;
        user_input.button = event.button.button;
        user_input.clicks = event.button.clicks;
        process_user_input(user_input);
      } break;
      case SDL_MOUSEWHEEL: {
        auto user_input = gp::common::UserInput{event.wheel.type, event.wheel.timestamp};
        user_input.x = event.wheel.x;
        user_input.y = event.wheel.y;
        user_input.x_float = event.wheel.preciseX;
        user_input.y_float = event.wheel.preciseY;
        process_user_input(user_input);
      } break;
      case SDL_KEYDOWN:
      case SDL_KEYUP: {
        auto user_input = gp::common::UserInput{event.key.type, event.key.timestamp};
        user_input.state = event.key.state;
        user_input.repeat = event.key.repeat;
        user_input.keysym_scancode = event.key.keysym.scancode;
        user_input.keysym_sym = event.key.keysym.sym;
        user_input.keysym_mod = event.key.keysym.mod;
        process_user_input(user_input);
      } break;
      default:
        break;
      }
    }
  }

  animation_.reset();
  sdl_sys_.reset();
}

void Renderer::init_rendering() {
  const auto wnd_title = std::string("assimp_loader");
  sdl_sys_ = std::make_unique<gp::sdl::System>(SDL_INIT_EVERYTHING,
                                               wnd_title.c_str(),
                                               0,
                                               30,
                                               width_,
                                               height_,
                                               SDL_WINDOW_OPENGL,
                                               -1,
                                               SDL_RENDERER_ACCELERATED);

  glewInit();

  glEnable(GL_DEPTH_TEST);
  glFrontFace(GL_CCW);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glViewport(0, 0, width_, height_);

  glReadBuffer(GL_BACK);

  projection_ = glm::perspective(glm::radians(60.0f), static_cast<float>(width_) / height_, 0.001f, 10240.0f);
  camera_pos_ = glm::vec3{0.0f, 1.0f, 6.0f};
  camera_rot_ = glm::vec3{0.0f, 0.0f, 0.0f};

  shader_program_ = load_shader_program("shaders/shader_program");
  configure_program();

  animation_ = std::make_unique<gp::sdl::Animation>(fps_);
}

void Renderer::animate(Uint32 time_elapsed_ms) {
  const float speed_factor = static_cast<float>(time_elapsed_ms) / 100.0f;

  const auto lg = std::lock_guard{mutex_};
  if (animate_) {
    camera_rot_.x += speed_factor;
    camera_rot_.y += speed_factor;
  }
}

void Renderer::configure_program() {
  const auto vertices_buf_location = glGetAttribLocation(shader_program_, "vertices_buf");
  glEnableVertexAttribArray(vertices_buf_location);

  for (std::size_t i = 0; i < model_->size(); i++) {
    const auto [vertices, indices, color] = model_->get(i);

    vertices_bufs_.emplace_back();
    glGenBuffers(1, &vertices_bufs_.back());
    glBindBuffer(GL_ARRAY_BUFFER, vertices_bufs_.back());
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), glm::value_ptr(vertices[0]), GL_STATIC_DRAW);
    glVertexAttribPointer(vertices_buf_location, 4, GL_FLOAT, GL_FALSE, 0, 0);

    indices_bufs_.emplace_back();
    glGenBuffers(1, &indices_bufs_.back());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_bufs_.back());
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]), indices.data(), GL_STATIC_DRAW);

    colors_.emplace_back(color);
    sizes_.emplace_back(indices.size());
  }

  color_location_ = glGetUniformLocation(shader_program_, "color");
  viewport_location_ = glGetUniformLocation(shader_program_, "viewport");
  camera_rot_location_ = glGetUniformLocation(shader_program_, "camera_rot");
}

GLuint Renderer::load_shader_program(const std::string &program_name) {
  const auto vert_shader_code = gp::utils::load_txt_file(program_name + ".vs");
  const auto vert_shader_code_c_str = vert_shader_code.c_str();

  const auto vert_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vert_shader, 1, &vert_shader_code_c_str, NULL);
  glCompileShader(vert_shader);

  check_shader_status(vert_shader, GL_COMPILE_STATUS);

  const auto frag_shader_code = gp::utils::load_txt_file(program_name + ".fs");
  const auto frag_shader_code_c_str = frag_shader_code.c_str();

  const auto frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(frag_shader, 1, &frag_shader_code_c_str, NULL);
  glCompileShader(frag_shader);

  check_shader_status(frag_shader, GL_COMPILE_STATUS);

  const auto shader_program = glCreateProgram();

  glAttachShader(shader_program, vert_shader);
  glAttachShader(shader_program, frag_shader);
  glLinkProgram(shader_program);

  check_program_status(shader_program, GL_LINK_STATUS);

  glDetachShader(shader_program, vert_shader);
  glDetachShader(shader_program, frag_shader);

  glDeleteShader(vert_shader);
  glDeleteShader(frag_shader);

  return shader_program;
}

void Renderer::check_shader_status(GLuint shader, GLenum status) {
  auto result = GL_FALSE;
  auto info_log_length = 0;

  glGetShaderiv(shader, status, &result);
  glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_length);
  if (info_log_length > 0) {
    auto error_message = std::string(info_log_length + 1, '\0');
    glGetShaderInfoLog(shader, info_log_length, NULL, error_message.data());
    printf("Shader error:\n%s\n", error_message.c_str());
  }
}

void Renderer::check_program_status(GLuint program, GLenum status) {
  auto result = GL_FALSE;
  auto info_log_length = 0;

  glGetProgramiv(program, status, &result);
  glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_length);
  if (info_log_length > 0) {
    auto error_message = std::string(info_log_length + 1, '\0');
    glGetProgramInfoLog(program, info_log_length, NULL, error_message.data());
    printf("Program error:\n%s\n", error_message.c_str());
  }
}
