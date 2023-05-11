#include "player_state.hpp"

#include "tools/math/math.hpp"

#include <cmath>
#include <numbers>

namespace wolf {
PlayerState::PlayerState(std::shared_ptr<const RawMap> raw_map, float fov_deg, float move_speed, float rot_speed)
    : raw_map_{raw_map}, fov_rad_{static_cast<float>(fov_deg * (std::numbers::pi / 180.0f))}, move_speed_{move_speed},
      rot_speed_{rot_speed} {
  const auto [w, h] = PlayerState::raw_map().player_pos();
  pos_x_ = w + 0.5f;
  pos_y_ = h + 0.5f;

  const auto &pos_type = PlayerState::raw_map().block(w, h);
  if (pos_type == 'n') {
    orientation_ = std::numbers::pi / 2.0f;
  } else if (pos_type == 's') {
    orientation_ = std::numbers::pi * 1.5f;
  } else if (pos_type == 'w') {
    orientation_ = std::numbers::pi;
  } else if (pos_type == 'e') {
    orientation_ = 0.0f;
  }
  update_dir();
}

void PlayerState::animate(std::uint32_t time_elapsed_ms) {
  animate_move(time_elapsed_ms);
  animate_rot(time_elapsed_ms);
}

float PlayerState::fov_deg() const { return fov_rad_ / (std::numbers::pi / 180.0f); }

float PlayerState::fov_rad() const { return fov_rad_; }

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

void PlayerState::animate_move(std::uint32_t time_elapsed_ms) {
  if (!keyboard_state().up() && !keyboard_state().down()) {
    return;
  }
  if (keyboard_state().up() && keyboard_state().down()) {
    return;
  }

  const auto time_elapsed_factor = time_elapsed_ms / 1000.0f;
  const auto move_factor = keyboard_state().up() ? move_speed() : -move_speed();
  pos_x_ += time_elapsed_factor * move_factor * dir_x();
  pos_y_ += time_elapsed_factor * move_factor * dir_y();
}

void PlayerState::animate_rot(std::uint32_t time_elapsed_ms) {
  if (!keyboard_state().left() && !keyboard_state().right()) {
    return;
  }
  if (keyboard_state().left() && keyboard_state().right()) {
    return;
  }

  const auto time_elapsed_factor = time_elapsed_ms / 1000.0f;
  orientation_ +=
      static_cast<float>(keyboard_state().left() ? -time_elapsed_factor : time_elapsed_factor) * rot_speed();
  orientation_ = tools::math::wrap_angle(orientation_);
  update_dir();
}

void PlayerState::update_dir() {
  dir_x_ = std::cosf(orientation());
  dir_y_ = std::sinf(orientation());
}
} // namespace wolf
