#include "encode_scene.hpp"

#include "streaming_common/constants.hpp"
#include "streaming_common/encoder.hpp"

#include <gp/gl/misc.hpp>
#include <gp/misc/event.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <array>

namespace streaming {
namespace {
template<typename... Ts>
constexpr auto make_array(Ts &&...args) {
  return std::array<std::common_type_t<Ts...>, sizeof...(Ts)>{std::forward<Ts>(args)...};
}
} // namespace

EncodeScene::EncodeScene(const VideoStreamInfo &video_stream_info)
    : encoder_(std::make_shared<Encoder>(video_stream_info))
    , ms_per_frame_(1000 / video_stream_info.fps) {
  Scene3D::init(video_stream_info.width, video_stream_info.height, "Streamer...");
}

EncodeScene::~EncodeScene() = default;

std::shared_ptr<Encoder> EncodeScene::encoder() const { return encoder_; }

void EncodeScene::loop(const gp::misc::Event &event) {
  switch (event.type()) {
  case gp::misc::Event::Type::Init:
    initialize();
    last_timestamp_ms_ = timestamp();
    break;
  case gp::misc::Event::Type::Quit:
    finalize();
    break;
  case gp::misc::Event::Type::Redraw: {
    process_event_queue();
    const auto time_elapsed_ms = event.timestamp() - last_timestamp_ms_;
    if (time_elapsed_ms >= ms_per_frame_) {
      animate(time_elapsed_ms);
      redraw();
      encode();
      swap_buffers();
      last_timestamp_ms_ = event.timestamp();
    }
  } break;
  case gp::misc::Event::Type::MouseButton:
    if (event.mouse_button().button == gp::misc::Event::MouseButton::Right &&
        event.mouse_button().action == gp::misc::Event::Action::Released) {
      animate_ = !animate_;
    }
    break;
  case gp::misc::Event::Type::MouseMove:
    if (event.mouse_move().left_is_down()) {
      animate_ = false;
      const auto speed_factor = 0.1f;
      camera_rot_.x += event.mouse_move().y_rel * speed_factor;
      camera_rot_.y += event.mouse_move().x_rel * speed_factor;
    }
    break;
  case gp::misc::Event::Type::MouseScroll: {
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
  default:
    break;
  }
}

void EncodeScene::handle_event(const gp::misc::Event &event) {
  const auto lock_guard = std::lock_guard(event_queue_mutex_);
  event_queue_.emplace_back(event);
}

void EncodeScene::initialize() {
  video_frame_ = encoder_->video_frame();
  init_scene();
}

void EncodeScene::finalize() {
  shader_program_.reset();
  indices_buffer_.reset();
  vertex_buffer_.reset();
  vao_.reset();
  video_frame_.reset();
  encoder_.reset();
}

void EncodeScene::process_event_queue() {
  auto event_queue = std::vector<gp::misc::Event>{};
  {
    const auto lock_guard = std::lock_guard(event_queue_mutex_);
    event_queue.swap(event_queue_);
  }
  for (const auto &event : event_queue) {
    loop(event);
  }
}

void EncodeScene::animate(const std::uint32_t time_elapsed_ms) {
  if (!animate_) {
    return;
  }

  const auto speed_factor = 0.05f;
  camera_rot_.x += time_elapsed_ms * speed_factor;
  camera_rot_.y += time_elapsed_ms * speed_factor;
}

void EncodeScene::redraw() {
  auto camera_rot_mat = glm::rotate(glm::mat4(1.0f), glm::radians(camera_rot_.x), glm::vec3(1.0f, 0.0f, 0.0f));
  camera_rot_mat = glm::rotate(camera_rot_mat, glm::radians(camera_rot_.y), glm::vec3(0.0f, 1.0f, 0.0f));
  camera_rot_mat = glm::rotate(camera_rot_mat, glm::radians(camera_rot_.z), glm::vec3(0.0f, 0.0f, 1.0f));
  const auto view = glm::lookAt(camera_pos_, glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{0.0f, 1.0f, 0.0f});
  const auto viewport_mat = projection_ * view;
  const auto mvp_mat = viewport_mat * camera_rot_mat;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  shader_program_->use();
  shader_program_->set_uniform("mvp", mvp_mat);

  vao_->bind();
  glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
}

void EncodeScene::encode() {
  constexpr auto format = CHANNELS_NUM == 4u ? GL_RGBA : GL_RGB;
  glReadPixels(0, 0, width(), height(), format, GL_UNSIGNED_BYTE, video_frame_->data());
  encoder_->encode();
}

void EncodeScene::init_scene() {
  glEnable(GL_DEPTH_TEST);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);

  glViewport(0, 0, width(), height());
  projection_ = glm::perspective(glm::radians(60.0f), static_cast<float>(width()) / height(), 1.0f, 1024.0f);

  camera_pos_ = glm::vec3{0.0f, 2.0f, 4.0f};
  camera_rot_ = glm::vec3{0.0f, 0.0f, 0.0f};

  // clang-format off
  const auto vertices = make_array(
    -1.0f, -1.0f, -1.0f,    1.0f, 0.0f, 0.0f,
     1.0f, -1.0f, -1.0f,    0.0f, 1.0f, 0.0f,
     1.0f,  1.0f, -1.0f,    0.0f, 0.0f, 1.0f,
    -1.0f,  1.0f, -1.0f,    0.0f, 1.0f, 1.0f,
    -1.0f, -1.0f,  1.0f,    1.0f, 1.0f, 0.0f,
    -1.0f,  1.0f,  1.0f,    1.0f, 0.0f, 1.0f,
     1.0f,  1.0f,  1.0f,    1.0f, 0.0f, 0.0f,
     1.0f, -1.0f,  1.0f,    0.0f, 1.0f, 1.0f);
  const auto indices = make_array(
    2u, 1u, 0u,
    0u, 3u, 2u,
    4u, 3u, 0u,
    5u, 3u, 4u,
    4u, 6u, 5u,
    4u, 7u, 6u,
    0u, 7u, 4u,
    1u, 7u, 0u,
    6u, 7u, 1u,
    1u, 2u, 6u,
    6u, 2u, 3u,
    3u, 5u, 6u);
  // clang-format on

  vao_ = std::make_unique<gp::gl::VertexArrayObject>();
  vao_->bind();

  vertex_buffer_ = std::make_unique<gp::gl::BufferObject>(GL_ARRAY_BUFFER);
  vertex_buffer_->bind();
  vertex_buffer_->set_data(vertices.size() * sizeof(vertices[0]), vertices.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  indices_buffer_ = std::make_unique<gp::gl::BufferObject>(GL_ELEMENT_ARRAY_BUFFER);
  indices_buffer_->bind();
  indices_buffer_->set_data(indices.size() * sizeof(indices[0]), indices.data(), GL_STATIC_DRAW);

  shader_program_ = gp::gl::create_shader_program("shaders/color_cube");
}
} // namespace streaming
