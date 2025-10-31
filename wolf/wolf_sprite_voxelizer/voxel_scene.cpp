#include "voxel_scene.hpp"

#include <gp/gl/misc.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace wolf {
VoxelScene::VoxelScene(std::shared_ptr<SpriteVoxelizer> sprite_voxelizer)
    : sprite_voxelizer_(std::move(sprite_voxelizer)) {}

void VoxelScene::loop(const gp::misc::Event &event) {
  switch (event.type()) {
  case gp::misc::Event::Type::Init:
    initialize(event.init().width, event.init().height);
    last_timestamp_ms_ = event.timestamp();

    voxelize_sprite();
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
    if (event.mouse_button().action == gp::misc::Event::Action::Pressed) {
      if (event.mouse_button().button == gp::misc::Event::MouseButton::Left) {
        animate_ = false;
      } else if (event.mouse_button().button == gp::misc::Event::MouseButton::Right) {
        voxelize_sprite();
      }
    }
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
  default:
    break;
  }
}

void VoxelScene::initialize(const int width, const int height) {
  glEnable(GL_DEPTH_TEST);
  glFrontFace(GL_CCW);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glClearColor(0.15f, 0.15f, 0.15f, 1.0f);

  camera_pos_ = glm::vec3{0.0f, 15.0f, 90.0f};
  camera_rot_ = glm::vec3{0.0f, 0.0f, 0.0f};

  resize(width, height);

  shader_program_ = gp::gl::create_shader_program("shaders/shader_program");
}

void VoxelScene::finalize() {
  shader_program_.reset();
  size_ = 0;
  index_buffer_.reset();
  vertex_buffer_.reset();
  vao_.reset();
}

void VoxelScene::resize(const int width, const int height) {
  glViewport(0, 0, width, height);
  projection_ = glm::perspective(glm::radians(60.0f), static_cast<float>(width) / height, 0.1f, 1000.0f);
}

void VoxelScene::animate(const std::uint32_t timestamp) {
  const auto time_elapsed_ms = timestamp - last_timestamp_ms_;

  if (animate_) {
    const auto speed_factor = static_cast<float>(time_elapsed_ms) / 15.0f;
    // camera_rot_.x += speed_factor;
    camera_rot_.y += speed_factor;
  }

  last_timestamp_ms_ = timestamp;
}

void VoxelScene::redraw() {
  auto camera_rot_mat = glm::rotate(glm::mat4(1.0f), glm::radians(camera_rot_.x), glm::vec3(1.0f, 0.0f, 0.0f));
  camera_rot_mat = glm::rotate(camera_rot_mat, glm::radians(camera_rot_.y), glm::vec3(0.0f, 1.0f, 0.0f));
  camera_rot_mat = glm::rotate(camera_rot_mat, glm::radians(camera_rot_.z), glm::vec3(0.0f, 0.0f, 1.0f));

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  shader_program_->use();
  shader_program_->set_uniform("camera_rot", camera_rot_mat);

  const auto view = glm::lookAt(camera_pos_, glm::vec3(0, -1, 0), glm::vec3(0, 1, 0));
  const auto viewport_mat = projection_ * view;
  shader_program_->set_uniform("viewport", viewport_mat);

  if (vao_) {
    vao_->bind();
    glDrawElements(GL_TRIANGLES, size_, GL_UNSIGNED_INT, 0);
  }
}

void VoxelScene::voxelize_sprite() {
  // Soldier model:
  //
  // const auto index = sprite_index_; // 50
  // sprite_index_ += 8;
  // sprite_voxelizer_->voxelize(index, index + 4, index + 2, index + 6);
  sprite_voxelizer_->voxelize(sprite_index_++, true);
  vipe_data();
  upload_data();
}

void VoxelScene::vipe_data() {
  vao_.reset();
  vertex_buffer_.reset();
  index_buffer_.reset();
  size_ = 0u;
}

void VoxelScene::upload_data() {
  vao_ = std::make_unique<gp::gl::VertexArrayObject>();

  vertex_buffer_ = std::make_unique<gp::gl::BufferObject>(GL_ARRAY_BUFFER);
  index_buffer_ = std::make_unique<gp::gl::BufferObject>(GL_ELEMENT_ARRAY_BUFFER);

  vao_->bind();

  vertex_buffer_->bind();
  vertex_buffer_->set_data(sprite_voxelizer_->vertex_data().size() * sizeof(SpriteVoxelizer::VoxelVertexData),
                           sprite_voxelizer_->vertex_data().data(),
                           GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SpriteVoxelizer::VoxelVertexData), nullptr);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1,
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(SpriteVoxelizer::VoxelVertexData),
                        reinterpret_cast<void *>(sizeof(glm::vec3)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2,
                        4,
                        GL_UNSIGNED_BYTE,
                        GL_TRUE,
                        sizeof(SpriteVoxelizer::VoxelVertexData),
                        reinterpret_cast<void *>(sizeof(glm::vec3) * 2));
  glEnableVertexAttribArray(2);

  index_buffer_->bind();
  index_buffer_->set_data(sprite_voxelizer_->indices().size() * sizeof(GLuint),
                          sprite_voxelizer_->indices().data(),
                          GL_STATIC_DRAW);

  size_ = static_cast<GLsizei>(sprite_voxelizer_->indices().size());
}
} // namespace wolf
