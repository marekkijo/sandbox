#include "model_scene.hpp"

#include <gp/glfw/misc.hpp>
#include <gp/misc/event.hpp>
#include <gp/utils/utils.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

ModelScene::ModelScene(std::shared_ptr<const Model> model)
    : model_{model} {}

void ModelScene::loop(const gp::misc::Event &event) {
  switch (event.type()) {
  case gp::misc::Event::Type::Init:
    last_timestamp_ms_ = event.timestamp();
    initialize(event.init().width, event.init().height);
    break;
  case gp::misc::Event::Type::Quit:
    break;
  case gp::misc::Event::Type::Resize:
    resize(event.resize().width, event.resize().height);
    break;
  case gp::misc::Event::Type::Redraw:
    animate(event.timestamp());
    redraw();
    break;
  case gp::misc::Event::Type::MouseButton:
    animate_ = false;
    break;
  case gp::misc::Event::Type::MouseMove:
    if (event.mouse_move().left_is_down()) {
      const auto speed_factor = 0.1f;
      camera_rot_.x += event.mouse_move().y_rel * speed_factor;
      camera_rot_.y += event.mouse_move().x_rel * speed_factor;
    }
    break;
  case gp::misc::Event::Type::MouseScroll: {
    animate_ = false;

    const auto speed_factor = 0.1f;
    const auto zoom_amount = std::fabsf(event.mouse_scroll().vertical) * speed_factor;
    const auto zoom_direction = event.mouse_scroll().vertical < 0.0f;
    if (zoom_direction) {
      camera_pos_ *= 1.0f + zoom_amount;
    } else {
      camera_pos_ *= 1.0f - zoom_amount;
    }
  } break;
  case gp::misc::Event::Type::Key:
    break;
  case gp::misc::Event::Type::DragDrop:
    break;
  default:
    break;
  }
}

void ModelScene::initialize(const int width, const int height) {
  glEnable(GL_DEPTH_TEST);
  glFrontFace(GL_CCW);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glReadBuffer(GL_BACK);

  camera_pos_ = glm::vec3{0.0f, 1.0f, 6.0f};
  camera_rot_ = glm::vec3{0.0f, 0.0f, 0.0f};

  resize(width, height);

  shader_program_ = load_shader_program("shaders/shader_program");
  configure_program();
}

void ModelScene::resize(const int width, const int height) {
  glViewport(0, 0, width, height);
  projection_ = glm::perspective(glm::radians(60.0f), static_cast<float>(width) / height, 0.001f, 8192.0f);
}

void ModelScene::animate(const std::uint32_t timestamp) {
  const auto time_elapsed_ms = timestamp - last_timestamp_ms_;

  if (animate_) {
    const auto speed_factor = static_cast<float>(time_elapsed_ms) / 100.0f;
    camera_rot_.x += speed_factor;
    camera_rot_.y += speed_factor;
  }

  last_timestamp_ms_ = timestamp;
}

void ModelScene::redraw() {
  glUseProgram(shader_program_);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  auto camera_rot_mat = glm::rotate(glm::mat4(1.0f), glm::radians(camera_rot_.x), glm::vec3(1.0f, 0.0f, 0.0f));
  camera_rot_mat = glm::rotate(camera_rot_mat, glm::radians(camera_rot_.y), glm::vec3(0.0f, 1.0f, 0.0f));
  camera_rot_mat = glm::rotate(camera_rot_mat, glm::radians(camera_rot_.z), glm::vec3(0.0f, 0.0f, 1.0f));
  glUniformMatrix4fv(camera_rot_location_, 1, GL_FALSE, glm::value_ptr(camera_rot_mat));

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
}

void ModelScene::configure_program() {
  gp::glfw::gl_debug(__FILE__, __LINE__);
  const auto vertices_buf_location = glGetAttribLocation(shader_program_, "vertices_buf");
  gp::glfw::gl_debug(__FILE__, __LINE__);
  glEnableVertexAttribArray(vertices_buf_location);
  gp::glfw::gl_debug(__FILE__, __LINE__);

  for (std::size_t i = 0; i < model_->size(); i++) {
    const auto [vertices, indices, color] = model_->get(i);

    vertices_bufs_.emplace_back();
    glGenBuffers(1, &vertices_bufs_.back());
    gp::glfw::gl_debug(__FILE__, __LINE__);
    glBindBuffer(GL_ARRAY_BUFFER, vertices_bufs_.back());
    gp::glfw::gl_debug(__FILE__, __LINE__);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), glm::value_ptr(vertices[0]), GL_STATIC_DRAW);
    gp::glfw::gl_debug(__FILE__, __LINE__);
    glVertexAttribPointer(vertices_buf_location, 4, GL_FLOAT, GL_FALSE, 0, 0);
    gp::glfw::gl_debug(__FILE__, __LINE__);

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

GLuint ModelScene::load_shader_program(const std::string &program_name) {
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

void ModelScene::check_shader_status(GLuint shader, GLenum status) {
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

void ModelScene::check_program_status(GLuint program, GLenum status) {
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
