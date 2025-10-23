#include "vswap_file_viewer_scene.hpp"

#include <gp/gl/misc.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace {
template<typename... Ts>
constexpr auto make_array(Ts &&...args) {
  return std::array<std::common_type_t<Ts...>, sizeof...(Ts)>{std::forward<Ts>(args)...};
}
} // namespace

VswapFileViewerScene::VswapFileViewerScene(std::shared_ptr<const wolf::VswapFile> vswap_file)
    : vswap_file_(std::move(vswap_file)) {}

void VswapFileViewerScene::loop(const gp::misc::Event &event) {
  switch (event.type()) {
  case gp::misc::Event::Type::Init:
    initialize(event.init().width, event.init().height);
    break;
  case gp::misc::Event::Type::Quit:
    finalize();
    break;
  case gp::misc::Event::Type::Resize:
    resize(event.resize().width, event.resize().height);
    break;
  case gp::misc::Event::Type::Redraw:
    redraw();
    swap_buffers();
    break;
  case gp::misc::Event::Type::MouseButton:
    if (event.mouse_button().action == gp::misc::Event::Action::Pressed) {
      if (event.mouse_button().button == gp::misc::Event::MouseButton::Left) {
        upload_wall();
      } else if (event.mouse_button().button == gp::misc::Event::MouseButton::Right) {
        upload_sprite();
      }
    }
    break;
  default:
    break;
  }
}

void VswapFileViewerScene::initialize(const int width, const int height) {
  glEnable(GL_DEPTH_TEST);
  glFrontFace(GL_CCW);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  resize(width, height);

  // clang-format off
  const auto vertices = make_array(
    -1.0f, -1.0f,    0.0f, 1.0f,
     1.0f, -1.0f,    1.0f, 1.0f,
     1.0f,  1.0f,    1.0f, 0.0f,
    -1.0f,  1.0f,    0.0f, 0.0f);
  // clang-format on

  vao_ = std::make_unique<gp::gl::VertexArrayObject>();
  vao_->bind();

  vertex_buffer_ = std::make_unique<gp::gl::BufferObject>(GL_ARRAY_BUFFER);
  vertex_buffer_->bind();
  vertex_buffer_->set_data(vertices.size() * sizeof(vertices[0]), vertices.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), nullptr);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), reinterpret_cast<void *>(2 * sizeof(GLfloat)));
  glEnableVertexAttribArray(1);

  frame_texture_ = std::make_unique<gp::gl::TextureObject>(GL_TEXTURE_2D);
  frame_texture_->bind();
  frame_texture_->set_parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  frame_texture_->set_parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  frame_texture_->set_parameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  frame_texture_->set_parameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  shader_program_ = gp::gl::create_shader_program("shaders/texture_screen");
  shader_program_->use();
  shader_program_->set_uniform("tex", 0);
}

void VswapFileViewerScene::finalize() {
  shader_program_.reset();
  frame_texture_.reset();
  vertex_buffer_.reset();
  vao_.reset();
}

void VswapFileViewerScene::resize(const int width, const int height) { glViewport(0, 0, width, height); }

void VswapFileViewerScene::redraw() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glActiveTexture(GL_TEXTURE0);
  shader_program_->use();
  frame_texture_->bind();
  vao_->bind();
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void VswapFileViewerScene::upload_wall() {
  auto index = wall_index_++;

  if (index >= vswap_file_->walls().size()) {
    index = wall_index_ = 0;
  }

  printf("Wall index: %zu\n", index);
  const auto &data = vswap_file_->walls()[index];
  frame_texture_->bind();
  frame_texture_->set_image(0, GL_RGBA, 64, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, glm::value_ptr(data[0]));
}

void VswapFileViewerScene::upload_sprite() {
  auto index = sprite_index_++;

  if (index >= vswap_file_->sprites().size()) {
    index = sprite_index_ = 0;
  }

  printf("Sprite index: %zu\n", index);
  const auto &data = vswap_file_->sprites()[index];
  frame_texture_->bind();
  frame_texture_->set_image(0, GL_RGBA, 64, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, glm::value_ptr(data[0]));
}
