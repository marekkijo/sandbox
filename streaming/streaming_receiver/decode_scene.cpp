#include "decode_scene.hpp"

#include "streaming_common/constants.hpp"
#include "streaming_common/decoder.hpp"

#include <gp/gl/misc.hpp>
#include <gp/misc/event.hpp>

#include <array>
#include <chrono>
#include <cstring>

namespace streaming {
namespace {
template<typename... Ts>
constexpr auto make_array(Ts &&...args) {
  return std::array<std::common_type_t<Ts...>, sizeof...(Ts)>{std::forward<Ts>(args)...};
}
} // namespace

DecodeScene::DecodeScene()
    : decoder_{std::make_unique<Decoder>()} {}

void DecodeScene::init(const VideoStreamInfo &video_stream_info) {
  video_stream_info_ = video_stream_info;
  ms_per_frame_ = 1000 / video_stream_info.fps;
  const auto async = true;
  Scene3D::init(video_stream_info.width, video_stream_info.height, "Decoding...", async);
}

void DecodeScene::consume_data(const std::byte *data, const std::size_t size, const bool eof) {
  if (size > 0) {
    const auto async = true;
    decoder_->incoming_data(data, size, async);
  }
  if (eof) {
    decoder_->signal_eof();
  }
}

void DecodeScene::set_event_callback(std::function<void(const gp::misc::Event &event)> event_callback) {
  event_callback_ = std::move(event_callback);
}

#ifdef STREAMING_PIPELINE_STATS
void DecodeScene::set_stats_log(std::FILE *const out) noexcept { decode_stats_.set_output(out); }

void DecodeScene::set_max_stats_reports(const uint32_t n) noexcept { decode_stats_.set_max_reports(n); }
#endif

void DecodeScene::loop(const gp::misc::Event &event) {
  switch (event.type()) {
  case gp::misc::Event::Type::Init:
    initialize();
    break;
  case gp::misc::Event::Type::Quit:
    finalize();
    break;
  case gp::misc::Event::Type::Redraw:
    decode();
    if (redraw()) {
      swap_buffers();
    }
    break;
  case gp::misc::Event::Type::MouseButton:
  case gp::misc::Event::Type::MouseMove:
  case gp::misc::Event::Type::MouseScroll:
  case gp::misc::Event::Type::Key:
    if (event_callback_) {
      event_callback_(event);
    }
    break;
  default:
    break;
  }
}

void DecodeScene::initialize() {
  init_streaming();
  init_scene();

  const auto frame_size = static_cast<GLsizeiptr>(video_stream_info_.width) * video_stream_info_.height * CHANNELS_NUM;
  for (auto &pbo : pbo_) {
    pbo = std::make_unique<gp::gl::BufferObject>(GL_PIXEL_UNPACK_BUFFER);
    pbo->bind();
    pbo->set_data(frame_size, nullptr, GL_STREAM_DRAW);
    pbo->unbind();
  }
}

void DecodeScene::finalize() {
  pbo_[0].reset();
  pbo_[1].reset();
  shader_program_.reset();
  frame_texture_.reset();
  vertex_buffer_.reset();
  vao_.reset();
  display_frame_.reset();
  rgb_frame_.reset();
  decoder_.reset();
  decoder_ = std::make_unique<Decoder>();
}

void DecodeScene::decode() {
  // Drain all available frames per Redraw; only the latest is displayed.
  // This prevents a growing backlog from causing the receiver to lag behind
  // the real-time stream by an ever-increasing number of frames.
  for (;;) {
    const auto status = decoder_->decode();
    switch (status.code) {
    case Decoder::Status::Code::OK:
#ifdef STREAMING_PIPELINE_STATS
    {
      const auto &dec_t = decoder_->last_timings();
      pending_decode_frame_ = {.upload_us = dec_t.upload_us,
                               .receive_us = dec_t.receive_us,
                               .yuv_to_rgb_us = dec_t.yuv_to_rgb_us};
    }
#endif
      rgb_frame_->swap(*display_frame_);
      frame_ready_ = true;
      break;
    case Decoder::Status::Code::RETRY:
      break;
    case Decoder::Status::Code::EOS:
      request_close();
      return;
    case Decoder::Status::Code::NODATA:
      return;
    case Decoder::Status::Code::ERROR:
      request_close();
      return;
    }
  }
}

bool DecodeScene::redraw() {
  if (!frame_ready_) {
    return false;
  }

  constexpr auto format = CHANNELS_NUM == 4u ? GL_RGBA : GL_RGB;

#ifdef STREAMING_PIPELINE_STATS
  using Clock = std::chrono::steady_clock;
  const auto t0 = Clock::now();
#endif

  // Double-buffered PBO unpack: CPU fills write PBO while GPU uploads from read PBO.
  const auto write_idx = pbo_index_;
  const auto read_idx = 1 - pbo_index_;
  pbo_index_ = 1 - pbo_index_;

  pbo_[write_idx]->bind();
  auto *dst = static_cast<std::byte *>(pbo_[write_idx]->map(GL_WRITE_ONLY));
  if (dst) {
    std::memcpy(dst, display_frame_->data(), display_frame_->size());
    pbo_[write_idx]->unmap();
  }
  pbo_[write_idx]->unbind();

  frame_texture_->bind();
  if (pbo_primed_) {
    // Kick async GPU texture upload from the previously filled PBO (returns immediately).
    pbo_[read_idx]->bind();
    frame_texture_->set_sub_image(0,
                                  0,
                                  0,
                                  video_stream_info_.width,
                                  video_stream_info_.height,
                                  format,
                                  GL_UNSIGNED_BYTE,
                                  nullptr);
    pbo_[read_idx]->unbind();
  } else {
    // First frame: read PBO not yet filled — upload directly so the first frame is visible.
    frame_texture_->set_sub_image(0,
                                  0,
                                  0,
                                  video_stream_info_.width,
                                  video_stream_info_.height,
                                  format,
                                  GL_UNSIGNED_BYTE,
                                  display_frame_->data());
    pbo_primed_ = true;
  }

#ifdef STREAMING_PIPELINE_STATS
  const auto t1 = Clock::now();
  pending_decode_frame_.texture_upload_us = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0);
  const auto t_draw0 = t1;
#endif

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glActiveTexture(GL_TEXTURE0);
  shader_program_->use();
  frame_texture_->bind();
  vao_->bind();
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

#ifdef STREAMING_PIPELINE_STATS
  pending_decode_frame_.display_us = std::chrono::duration_cast<std::chrono::microseconds>(Clock::now() - t_draw0);
  if (decode_stats_.record(pending_decode_frame_)) {
    request_close();
  }
#endif

  frame_ready_ = false;
  return true;
}

void DecodeScene::init_streaming() {
  decoder_->init(video_stream_info_);
  rgb_frame_ = decoder_->rgb_frame();
  display_frame_ = std::make_shared<FrameData>(video_stream_info_.width * video_stream_info_.height * CHANNELS_NUM);
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

  constexpr auto fmt = CHANNELS_NUM == 4u ? GL_RGBA : GL_RGB;
  frame_texture_
      ->set_image(0, fmt, video_stream_info_.width, video_stream_info_.height, 0, fmt, GL_UNSIGNED_BYTE, nullptr);

  shader_program_ = gp::gl::create_shader_program("shaders/texture_screen");
  shader_program_->use();
  shader_program_->set_uniform("tex", 0);
}
} // namespace streaming
