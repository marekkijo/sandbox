#include "model_scene.hpp"

#include <gp/gl/misc.hpp>

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
    finalize();
    break;
  case gp::misc::Event::Type::Resize:
    resize(event.resize().width, event.resize().height);
    break;
  case gp::misc::Event::Type::Redraw:
    animate(event.timestamp());
    redraw();
    swap_buffers();
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

  upload_data();
  shader_program_ = gp::gl::create_shader_program("shaders/shader_program");
}

void ModelScene::finalize() {
  shader_program_.reset();
  sizes_.clear();
  colors_.clear();
  indices_buffers_.reset();
  normals_buffers_.reset();
  vertices_buffers_.reset();
  vaos_.reset();
}

void ModelScene::resize(const int width, const int height) {
  glViewport(0, 0, width, height);
  projection_ = glm::perspective(glm::radians(60.0f), static_cast<float>(width) / height, 1.1f, 819200.0f);
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
  auto camera_rot_mat = glm::rotate(glm::mat4(1.0f), glm::radians(camera_rot_.x), glm::vec3(1.0f, 0.0f, 0.0f));
  camera_rot_mat = glm::rotate(camera_rot_mat, glm::radians(camera_rot_.y), glm::vec3(0.0f, 1.0f, 0.0f));
  camera_rot_mat = glm::rotate(camera_rot_mat, glm::radians(camera_rot_.z), glm::vec3(0.0f, 0.0f, 1.0f));

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  shader_program_->use();
  shader_program_->set_uniform("camera_rot", camera_rot_mat);

  const auto view = glm::lookAt(camera_pos_, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
  const auto viewport_mat = projection_ * view;
  shader_program_->set_uniform("viewport", viewport_mat);

  for (std::size_t i = 0; i < number_of_meshes_; i++) {
    vaos_->bind(i);
    shader_program_->set_uniform("color", colors_[i]);

    glDrawElements(GL_TRIANGLES, sizes_[i], GL_UNSIGNED_INT, 0);
  }
}

void ModelScene::upload_data() {
  number_of_meshes_ = model_->size();

  vaos_ = std::make_unique<gp::gl::VertexArrayObjects>(number_of_meshes_);

  vertices_buffers_ = std::make_unique<gp::gl::BufferObjects>(number_of_meshes_, GL_ARRAY_BUFFER);
  normals_buffers_ = std::make_unique<gp::gl::BufferObjects>(number_of_meshes_, GL_ARRAY_BUFFER);
  indices_buffers_ = std::make_unique<gp::gl::BufferObjects>(number_of_meshes_, GL_ELEMENT_ARRAY_BUFFER);

  colors_.reserve(number_of_meshes_);
  sizes_.reserve(number_of_meshes_);

  for (std::size_t i = 0; i < number_of_meshes_; i++) {
    const auto [vertices, normals, indices, color] = model_->get(i);

    vaos_->bind(i);

    vertices_buffers_->bind(i);
    vertices_buffers_->set_data(vertices.size() * sizeof(vertices[0]), glm::value_ptr(vertices[0]), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    normals_buffers_->bind(i);
    normals_buffers_->set_data(normals.size() * sizeof(normals[0]), glm::value_ptr(normals[0]), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);

    indices_buffers_->bind(i);
    indices_buffers_->set_data(indices.size() * sizeof(indices[0]), indices.data(), GL_STATIC_DRAW);

    colors_.emplace_back(color);
    sizes_.emplace_back(static_cast<GLsizei>(indices.size()));
  }
}
