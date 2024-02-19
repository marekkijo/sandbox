#include "rgb_cube_gl_scene.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <array>
#include <chrono>
#include <string_view>

#include "gl.hpp"

RGBCubeGLScene::~RGBCubeGLScene() {
  gl::buffer::delete_buffer(vbo_);
  gl::buffer::delete_buffer(ibo_);
  gl::vao::delete_vao(vao_);
  gl::shader::delete_shader(shader_program_);
}

void RGBCubeGLScene::init() {
  init_buffer();
  init_shader();

  projection_ =
      glm::perspective(glm::radians(60.0f), static_cast<float>(window_width()) / window_height(), 1.0f, 1024.0f);
  camera_pos_ = glm::vec3{0.0f, 2.0f, 4.0f};
  camera_rot_ = glm::vec3{0.0f, 0.0f, 0.0f};
  view_ = glm::lookAt(camera_pos_, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
  viewport_mat_ = projection_ * view_;
}

void RGBCubeGLScene::draw() {
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glViewport(0, 0, window_width(), window_height());

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  auto camera_rot_mat = glm::rotate(glm::mat4(1.0f), glm::radians(camera_rot_.x), glm::vec3(1.0f, 0.0f, 0.0f));
  camera_rot_mat = glm::rotate(camera_rot_mat, glm::radians(camera_rot_.y), glm::vec3(0.0f, 1.0f, 0.0f));
  camera_rot_mat = glm::rotate(camera_rot_mat, glm::radians(camera_rot_.z), glm::vec3(0.0f, 0.0f, 1.0f));

  const auto mvp_mat = viewport_mat_ * camera_rot_mat;

  glUseProgram(shader_program_);
  glUniformMatrix4fv(mvp_location_, 1, GL_FALSE, glm::value_ptr(mvp_mat));

  glBindVertexArray(vao_);
  glDrawElements(GL_TRIANGLES, num_indices_, GL_UNSIGNED_INT, nullptr);
  glBindVertexArray(0);

  glUseProgram(0);

  static auto last_timestamp = std::chrono::steady_clock::now();

  static decltype(std::chrono::steady_clock::now()) current_timestamp;
  current_timestamp = std::chrono::steady_clock::now();

  const auto time_elapsed = current_timestamp - last_timestamp;
  animate(std::chrono::duration_cast<std::chrono::milliseconds>(time_elapsed).count());
  last_timestamp = current_timestamp;
}

void RGBCubeGLScene::process_user_input(const gp::common::UserInput2 &user_input) {
  switch (user_input.type) {
  case gp::common::UserInput2::Type::MouseMotion: {
    if ((user_input.state & gp::common::UserInput2::Mask::LeftMouseButton) != 0) {
      static constexpr auto speed_factor = 0.1;

      const auto lg = std::lock_guard{mutex_};
      camera_rot_.x += static_cast<float>(user_input.y_relative * speed_factor);
      camera_rot_.y += static_cast<float>(user_input.x_relative * speed_factor);
    }
    break;
  }
  case gp::common::UserInput2::Type::MouseButtonDown: {
    const auto lg = std::lock_guard{mutex_};
    animate_ = false;
    break;
  }
  case gp::common::UserInput2::Type::MouseButtonUp: {
    const auto lg = std::lock_guard{mutex_};
    animate_ = true;
    break;
  }
  default:
    break;
  }
}

void RGBCubeGLScene::toggle_animation_speed_factor() {
  if (animation_speed_factor_ == 0.01f) {
    animation_speed_factor_ = 0.1f;
  } else {
    animation_speed_factor_ = 0.01f;
  }
}

void RGBCubeGLScene::init_buffer() {
  constexpr auto indices_data = std::array<GLuint, 36u>{2, 1, 0, 0, 3, 2, 4, 3, 0, 5, 3, 4, 4, 6, 5, 4, 7, 6,
                                                        0, 7, 4, 1, 7, 0, 6, 7, 1, 1, 2, 6, 6, 2, 3, 3, 5, 6};
  ibo_ = gl::buffer::make_index_buffer(indices_data.size() * sizeof(GLuint), indices_data.data());
  num_indices_ = indices_data.size();

  constexpr auto vertex_data = std::array<GLfloat, 48u>{
      -1.0f, -1.0f, -1.0f, 1.0f, 0.0f,  0.0f, 1.0f, -1.0f, -1.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f, -1.0f, 0.0f,
      0.0f,  1.0f,  -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f,  -1.0f, -1.0f, 1.0f, 1.0f,  1.0f, 0.0f, -1.0f, 1.0f,
      1.0f,  1.0f,  0.0f,  1.0f, 1.0f,  1.0f, 1.0f, 1.0f,  0.0f,  0.0f,  1.0f, -1.0f, 1.0f, 0.0f, 1.0f,  1.0f};
  vbo_ = gl::buffer::make_vertex_buffer(vertex_data.size() * sizeof(GLfloat), vertex_data.data());

  constexpr GLsizei stride{6 * sizeof(GLfloat)};

  glGenVertexArrays(1, &vao_);
  glBindVertexArray(vao_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void *>(0));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void *>(3 * sizeof(GLfloat)));
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void RGBCubeGLScene::init_shader() {
  constexpr std::string_view vert_src = "#version 330 core\n"
                                        ""
                                        "layout(location = 0) in vec3 pos_in;"
                                        "layout(location = 1) in vec3 color_in;"
                                        ""
                                        "uniform mat4 mvp;"
                                        ""
                                        "out vec3 color_frag;"
                                        ""
                                        "void main()"
                                        "{"
                                        "  color_frag = color_in;"
                                        "  gl_Position = mvp * vec4(pos_in, 1);"
                                        "}";

  constexpr std::string_view frag_src = "#version 330 core\n"
                                        ""
                                        "#ifdef GL_ES\n"
                                        "precision mediump float;\n"
                                        "#endif\n"
                                        ""
                                        "in vec3 color_frag;"
                                        ""
                                        "out vec4 color_out;"
                                        ""
                                        "void main()"
                                        "{"
                                        "  color_out = vec4(color_frag, 1);"
                                        "}";

  shader_program_ = gl::shader::make_shader(vert_src, frag_src);
  mvp_location_ = glGetUniformLocation(shader_program_, "mvp");
}

void RGBCubeGLScene::animate(int time_elapsed_ms) {
  if (!animate_) { return; }

  const auto lg = std::lock_guard{mutex_};
  camera_rot_.y += static_cast<float>(time_elapsed_ms) * animation_speed_factor_;
}
