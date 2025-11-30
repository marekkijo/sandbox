#include "pong_scene.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

#include <sstream>

namespace ai {
namespace {
constexpr auto paddle_width = 0.15f;
constexpr auto paddle_height = 0.025f;
constexpr auto paddle_speed = 0.0005f;

constexpr auto ball_radius = 0.01f;
constexpr auto ball_speed = 0.0005f;
} // namespace

PongScene::PongScene() {
  for (auto &neural_network : neural_networks_) {
    neural_network = std::make_unique<NeuralNetwork>(5, std::vector<std::size_t>{4, 4, 1});
    neural_network->init_random(0.0f, 1.0f);
  }
}

void PongScene::loop(const gp::misc::Event &event) {
  switch (event.type()) {
  case gp::misc::Event::Type::Init:
    initialize(event.init().width, event.init().height);
    last_timestamp_ms_ = event.timestamp();
    break;
  case gp::misc::Event::Type::Quit:
    finalize();
    break;
  case gp::misc::Event::Type::Resize:
    resize(event.resize().width, event.resize().height);
    break;
  case gp::misc::Event::Type::Redraw: {
    const auto time_elapsed_ms = event.timestamp() - last_timestamp_ms_;
    animate(time_elapsed_ms);
    last_timestamp_ms_ = event.timestamp();
    redraw();
  } break;
  default:
    break;
  }
}

void PongScene::initialize(const int width, const int height) { resize(width, height); }

void PongScene::finalize() {}

void PongScene::resize(const int width, const int height) {
  width_ = width;
  height_ = height;
}

void PongScene::animate(const std::uint64_t time_elapsed_ms) {
  if (keyboard_state()->is_down(gp::misc::Event::ScanCode::Left) !=
      keyboard_state()->is_down(gp::misc::Event::ScanCode::Right)) {
    if (keyboard_state()->is_down(gp::misc::Event::ScanCode::Left)) {
      game_states_[0].paddle_position -= paddle_speed * time_elapsed_ms;
    } else {
      game_states_[0].paddle_position += paddle_speed * time_elapsed_ms;
    }
    game_states_[0].paddle_position = glm::clamp(game_states_[0].paddle_position, 0.0f, 1.0f);
  }

  for (std::size_t i = 0; i < number_per_generation; i++) {
    auto &neural_network = neural_networks_[i];
    auto &game_state = game_states_[i];
    neural_network->set_input(0, game_state.paddle_position);
    neural_network->set_input(1, game_state.prev_ball_position.x);
    neural_network->set_input(2, game_state.prev_ball_position.y);
    neural_network->set_input(3, game_state.ball_position.x);
    neural_network->set_input(4, game_state.ball_position.y);
    neural_network->feed_forward();

    const auto output = neural_network->get_output();
    if (output[0] > 0.5f) {
      game_state.paddle_position -= paddle_speed * time_elapsed_ms;
    } else if (output[0] < 0.5f) {
      game_state.paddle_position += paddle_speed * time_elapsed_ms;
    }
    game_state.paddle_position = glm::clamp(game_state.paddle_position, 0.0f, 1.0f);
    game_state.prev_ball_position = game_state.ball_position;

    game_state.ball_angle = glm::normalize(game_state.ball_angle);
    game_state.ball_position += game_state.ball_angle * static_cast<float>(game_state.ball_speed * time_elapsed_ms);
    if (game_state.ball_position.x < 0.0f || game_state.ball_position.x > 1.0f) {
      game_state.ball_angle.x = -game_state.ball_angle.x;
      if (game_state.ball_position.x < 0.0f) {
        game_state.ball_position.x = -game_state.ball_position.x;
      } else {
        game_state.ball_position.x = 2.0f - game_state.ball_position.x;
      }
    }
    if (game_state.ball_position.y < 0.0f) {
      game_state.ball_angle.y = -game_state.ball_angle.y;
      game_state.ball_position.y = -game_state.ball_position.y;
    } else if (game_state.ball_position.y > 1.0f) {
      game_state.ball_position = {0.5f, 0.5f};
      game_state.ball_angle.y = -game_state.ball_angle.y;
      game_state.score = 0;
      game_state.ball_speed = 0.0005f;
    } else if (game_state.ball_position.y > (1.0f - paddle_height)) {
      if (game_state.ball_position.x >= game_state.paddle_position - paddle_width / 2 &&
          game_state.ball_position.x <= game_state.paddle_position + paddle_width / 2) {
        game_state.ball_angle.y = -0.2f;
        game_state.ball_angle.x = (game_state.ball_position.x - game_state.paddle_position) / paddle_width;
        game_state.ball_position.y = 2.0f * (1.0f - paddle_height) - game_state.ball_position.y;
        game_state.ball_speed *= 1.1f;
        game_state.score++;
      }
    }
  }
}

void PongScene::redraw() {
  r().set_color(0, 0, 0);
  r().clear();

  r().set_color(255, 255, 255);
  r().draw_line(0, height_ / 2, width_, height_ / 2);

  auto score_label_pos = glm::vec2{10, 10};
  for (std::size_t i = 0; i < number_per_generation; i++) {
    std::ostringstream score_label;
    score_label << "Score[" << i << "]: " << game_states_[i].score;
    r().draw_text(score_label.str(), score_label_pos.x, score_label_pos.y);
    score_label_pos.y += 20;
  }

  r().set_color(255, 255, 255, static_cast<int>(255.0f / number_per_generation + 0.5f));
  for (std::size_t i = 0; i < number_per_generation; i++) {
    const auto ball_rect = SDL_FRect{
        width_ * game_states_[i].ball_position.x - width_ * ball_radius,
        height_ * game_states_[i].ball_position.y - height_ * ball_radius,
        width_ * ball_radius * 2.0f,
        height_ * ball_radius * 2.0f,
    };
    r().fill_rect(ball_rect);
  }

  for (std::size_t i = 0; i < number_per_generation; i++) {
    const auto paddle_rect = SDL_FRect{
        width_ * game_states_[i].paddle_position - width_ * paddle_width / 2.0f,
        height_ - height_ * paddle_height,
        width_ * paddle_width,
        height_ * paddle_height,
    };
    r().fill_rect(paddle_rect);
  }

  r().present();
}
} // namespace ai
