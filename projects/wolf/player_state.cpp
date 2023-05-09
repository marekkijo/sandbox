#include "player_state.hpp"

#include <numbers>

namespace wolf {
PlayerState::PlayerState(std::shared_ptr<const RawMap> _raw_map, float move_speed, float rot_speed)
    : raw_map_{_raw_map}, move_speed_{move_speed}, rot_speed_{rot_speed} {
  const auto [w, h] = raw_map().player_pos();
  pos_x_ = w + 0.5f;
  pos_y_ = h + 0.5f;

  const auto &pos_type = raw_map().block(w, h);
  if (pos_type == 'n') {
    orientation_ = std::numbers::pi / 2.0f;
    dir_y_ = 1.0;
  } else if (pos_type == 's') {
    orientation_ = std::numbers::pi * 1.5f;
    dir_y_ = -1.0;
  } else if (pos_type == 'w') {
    orientation_ = std::numbers::pi;
    dir_x_ = -1.0;
  } else if (pos_type == 'e') {
    dir_x_ = 1.0;
  }
}

void PlayerState::animate(std::uint32_t time_elapsed_ms) {}

float PlayerState::pos_x() const { return pos_x_; }

float PlayerState::pos_y() const { return pos_y_; }

std::tuple<float, float> PlayerState::pos() const { return std::make_tuple(pos_x(), pos_y()); }

float PlayerState::orientation() const { return orientation_; }

float PlayerState::dir_x() const { return dir_x_; }

float PlayerState::dir_y() const { return dir_y_; }

std::tuple<float, float> PlayerState::dir() const { return std::make_tuple(dir_x(), dir_y()); }

const RawMap &PlayerState::raw_map() const { return *raw_map_; }

const tools::sdl::SDLKeyboardState &PlayerState::keyboard_state() const { return keyboard_state_; }

float PlayerState::move_speed() const { return move_speed_; }

float PlayerState::rot_speed() const { return rot_speed_; }
} // namespace wolf
