#include "decode_scene.hpp"

#include "streaming_common/constants.hpp"
#include "streaming_common/decoder.hpp"

#include <gp/gl/misc.hpp>
#include <gp/misc/event.hpp>

#include <array>

namespace streaming {
namespace {
template<typename... Ts>
constexpr auto make_array(Ts &&...args) {
  return std::array<std::common_type_t<Ts...>, sizeof...(Ts)>{std::forward<Ts>(args)...};
}
} // namespace

DecodeScene::DecodeScene(const VideoStreamInfo &video_stream_info)
    : video_stream_info_(video_stream_info)
    , ms_per_frame_(1000 / video_stream_info.fps)
    , decoder_{std::make_unique<Decoder>()} {
  SceneGL::init(video_stream_info.width, video_stream_info.height, "Decoding...");
}

DecodeScene::~DecodeScene() = default;

void DecodeScene::loop(const gp::misc::Event &event) {
  switch (event.type()) {
  case gp::misc::Event::Type::Init:
    initialize();
    last_timestamp_ms_ = timestamp();
    break;
  case gp::misc::Event::Type::Quit:
    finalize();
    break;
  case gp::misc::Event::Type::Redraw: {
    decode();
    const auto time_elapsed_ms = event.timestamp() - last_timestamp_ms_;
    if (time_elapsed_ms >= ms_per_frame_) {
      if (redraw()) {
        swap_buffers();
        last_timestamp_ms_ = timestamp();
      }
    }
  } break;
  default:
    break;
  }
}

void DecodeScene::initialize() {
  init_streaming();
  init_scene();
}

void DecodeScene::finalize() {
  shader_program_.reset();
  frame_texture_.reset();
  vertex_buffer_.reset();
  vao_.reset();
  rgb_frame_.reset();
  input_file_.reset();
  decoder_.reset();
  decoder_ = std::make_unique<Decoder>();
}

void DecodeScene::decode() {
  constexpr auto format = CHANNELS_NUM == 4u ? GL_RGBA : GL_RGB;

  if (frame_ready_) {
    return;
  }

  const auto status = decoder_->decode();
  switch (status.code) {
  case Decoder::Status::Code::OK:
    printf("Decoding: decoded frame: %i\n", status.frame_num);
    frame_texture_->bind();
    frame_texture_->set_image(0,
                              format,
                              video_stream_info_.width,
                              video_stream_info_.height,
                              0,
                              format,
                              GL_UNSIGNED_BYTE,
                              rgb_frame_->data());
    frame_ready_ = true;
    break;
  case Decoder::Status::Code::RETRY:
    printf("Decoding: packet sent, retrying...\n");
    break;
  case Decoder::Status::Code::EOS:
    printf("Decoding: end of stream\n");
    request_close();
    break;
  case Decoder::Status::Code::NODATA:
    printf("Decoding: no data left to decode\n");
    read_some();
    break;
  case Decoder::Status::Code::ERROR:
    printf("Decoding: ERROR\n");
    request_close();
    break;
  }
}

bool DecodeScene::redraw() {
  if (!frame_ready_) {
    return false;
  }

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glActiveTexture(GL_TEXTURE0);
  shader_program_->use();
  frame_texture_->bind();
  vao_->bind();
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
  frame_ready_ = false;
  return true;
}

void DecodeScene::init_streaming() {
  input_file_ = std::make_unique<std::ifstream>("file.h264", std::ios::in | std::ios::binary);
  decoder_->init(video_stream_info_);
  rgb_frame_ = decoder_->rgb_frame();
}

void DecodeScene::init_scene() {
  glEnable(GL_DEPTH_TEST);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);

  glViewport(0, 0, video_stream_info_.width, video_stream_info_.height);

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

void DecodeScene::read_some() {
  constexpr std::size_t buffer_size_1_kB = 1024u;

  if (input_file_->eof()) {
    return;
  }

  static std::array<std::byte, buffer_size_1_kB> buffer{};
  input_file_->read(reinterpret_cast<char *>(buffer.data()), buffer.size());

  const auto size = static_cast<std::size_t>(input_file_->gcount());
  decoder_->incoming_data(buffer.data(), size);

  if (input_file_->eof()) {
    decoder_->signal_eof();
  }
}
} // namespace streaming
