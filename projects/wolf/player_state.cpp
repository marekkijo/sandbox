#include "player_state.hpp"

#include "tools/math/math.hpp"

#include <cmath>
#include <numbers>

namespace wolf {
PlayerState::PlayerState(std::shared_ptr<const RawMap> raw_map, float fov_deg, float move_speed, float rot_speed)
    : raw_map_{raw_map}
    , fov_rad_{static_cast<float>(fov_deg * (std::numbers::pi / 180.0f))}
    , move_speed_{move_speed}
    , rot_speed_{rot_speed}
    , orientation_{deduce_orientation()} {
  const auto [w, h] = raw_map_->player_pos();
  pos_              = glm::vec2{w + 0.5f, h + 0.5f};
  update_dir();
}

void PlayerState::animate(std::uint32_t time_elapsed_ms) {
  animate_move(time_elapsed_ms);
  animate_rot(time_elapsed_ms);
}

float PlayerState::fov_rad() const { return fov_rad_; }

float PlayerState::orientation() const { return orientation_; }

const glm::vec2 &PlayerState::pos() const { return pos_; }

const glm::vec2 &PlayerState::dir() const { return dir_; }

float PlayerState::deduce_orientation() const {
  const auto [w, h]    = raw_map_->player_pos();
  const auto &pos_type = raw_map_->block(w, h);
  if (pos_type == 'n') {
    return std::numbers::pi / 2.0f;
  } else if (pos_type == 's') {
    return std::numbers::pi * 1.5f;
  } else if (pos_type == 'w') {
    return std::numbers::pi;
  } else {
    return 0.0f;
  }
}

void PlayerState::animate_move(std::uint32_t time_elapsed_ms) {
  if (!keyboard_state_.up() && !keyboard_state_.down()) { return; }
  if (keyboard_state_.up() && keyboard_state_.down()) { return; }

  const auto time_elapsed_factor = time_elapsed_ms / 1000.0f;
  const auto move_factor         = keyboard_state_.up() ? move_speed_ : -move_speed_;
  pos_ += glm::vec2(time_elapsed_factor * move_factor * dir().x, time_elapsed_factor * move_factor * dir().y);
}

void PlayerState::animate_rot(std::uint32_t time_elapsed_ms) {
  if (!keyboard_state_.left() && !keyboard_state_.right()) { return; }
  if (keyboard_state_.left() && keyboard_state_.right()) { return; }

  const auto time_elapsed_factor = time_elapsed_ms / 1000.0f;
  orientation_ += static_cast<float>(keyboard_state_.left() ? -time_elapsed_factor : time_elapsed_factor) * rot_speed_;
  orientation_  = tools::math::wrap_angle(orientation_);
  update_dir();
}

void PlayerState::update_dir() { dir_ = glm::vec2{std::cosf(orientation()), std::sinf(orientation())}; }
} // namespace wolf
