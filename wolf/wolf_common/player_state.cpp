#include "player_state.hpp"

#include "wolf_common/wolf_map_info.hpp"

#include <gp/math/math.hpp>

#include <cmath>
#include <numbers>

namespace wolf {
using ScanCode = gp::misc::Event::ScanCode;

PlayerState::PlayerState(const RawMap &raw_map, const float move_speed, const float rot_speed)
    : move_speed_{move_speed}
    , rot_speed_{rot_speed}
    , orientation_{deduce_orientation(raw_map)} {
  const auto [w, h] = raw_map.player_pos();
  pos_ = glm::vec2{w + 0.5f, h + 0.5f};
  update_dir();
}

void PlayerState::set_keyboard_state(std::shared_ptr<gp::misc::KeyboardState> keyboard_state) {
  keyboard_state_ = std::move(keyboard_state);
}

void PlayerState::animate(const std::uint32_t time_elapsed_ms) {
  animate_move(time_elapsed_ms);
  animate_rot(time_elapsed_ms);
}

float PlayerState::orientation() const { return orientation_; }

const glm::vec2 &PlayerState::pos() const { return pos_; }

const glm::vec2 &PlayerState::dir() const { return dir_; }

float PlayerState::deduce_orientation(const RawMap &raw_map) const {
  const auto [w, h] = raw_map.player_pos();
  const auto pos_type = raw_map.block(w, h).object;
  if (pos_type == Map::Objects::start_position_n) { return std::numbers::pi * 1.5f; }
  if (pos_type == Map::Objects::start_position_s) { return std::numbers::pi / 2.0f; }
  if (pos_type == Map::Objects::start_position_w) { return std::numbers::pi; }
  return 0.0f;
}

void PlayerState::animate_move(const std::uint32_t time_elapsed_ms) {
  if (keyboard_state_->is_down(ScanCode::Up) != keyboard_state_->is_down(ScanCode::Down)) {
    const auto time_elapsed_factor = time_elapsed_ms / 1000.0f;
    const auto move_factor = keyboard_state_->is_down(ScanCode::Up) ? move_speed_ : -move_speed_;
    pos_ += glm::vec2(time_elapsed_factor * move_factor * dir().x, time_elapsed_factor * move_factor * dir().y);
  }
}

void PlayerState::animate_rot(const std::uint32_t time_elapsed_ms) {
  if (keyboard_state_->is_down(ScanCode::Left) != keyboard_state_->is_down(ScanCode::Right)) {
    const auto time_elapsed_factor = time_elapsed_ms / 1000.0f;
    orientation_ +=
        static_cast<float>(keyboard_state_->is_down(ScanCode::Left) ? -time_elapsed_factor : time_elapsed_factor) *
        rot_speed_;
    orientation_ = gp::math::wrap_angle(orientation_);
    update_dir();
  }
}

void PlayerState::update_dir() { dir_ = glm::vec2{std::cosf(orientation()), std::sinf(orientation())}; }
} // namespace wolf
