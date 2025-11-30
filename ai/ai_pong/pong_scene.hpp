#pragma once

#include "ai_common/neural_network.hpp"

#include <gp/sdl/scene_2d.hpp>

#include <glm/vec2.hpp>

#include <cstdint>

namespace ai {
class PongScene : public gp::sdl::Scene2D {
public:
  PongScene();

private:
  struct GameState {
    int score{0};
    float paddle_position{0.5f};
    glm::vec2 ball_position{0.5f, 0.5f};
    glm::vec2 prev_ball_position{0.5f, 0.5f};
    glm::vec2 ball_angle{0.4f, -0.5f};
    float ball_speed{0.0005f};
  };

  constexpr static std::size_t number_per_generation{25};

  void loop(const gp::misc::Event &event) override;

  void initialize(const int width, const int height);
  void finalize();
  void resize(const int width, const int height);
  void animate(const std::uint64_t time_elapsed_ms);
  void redraw();

  std::uint64_t last_timestamp_ms_{};
  int width_{};
  int height_{};

  std::vector<GameState> game_states_{number_per_generation};
  std::vector<std::unique_ptr<NeuralNetwork>> neural_networks_{number_per_generation};
};
} // namespace ai
