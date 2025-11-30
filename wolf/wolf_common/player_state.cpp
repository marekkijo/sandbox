#include "player_state.hpp"

#include "wolf_common/wolf_map_info.hpp"

#include <gp/math/math.hpp>

#include <glm/glm.hpp>

#include <cmath>
#include <numbers>

namespace wolf {
using ScanCode = gp::misc::Event::ScanCode;

PlayerState::PlayerState(const RawMap &raw_map, const float move_speed, const float rot_speed)
    : raw_map_{raw_map}
    , move_speed_{move_speed}
    , rot_speed_{rot_speed}
    , orientation_{deduce_orientation(raw_map_)} {
  pos_ = glm::vec2{raw_map_.player_pos()} + glm::vec2{0.5f, 0.5f};
}

void PlayerState::set_keyboard_state(std::shared_ptr<gp::misc::KeyboardState> keyboard_state) {
  keyboard_state_ = std::move(keyboard_state);
}

void PlayerState::animate(const std::uint64_t time_elapsed_ms) {
  animate_move(time_elapsed_ms);
  animate_rot(time_elapsed_ms);
}

float PlayerState::orientation() const { return orientation_; }

glm::vec2 PlayerState::pos() const { return pos_; }

glm::vec2 PlayerState::dir() const { return glm::vec2{std::cosf(orientation()), std::sinf(orientation())}; }

float PlayerState::deduce_orientation(const RawMap &raw_map) const {
  const auto pos = raw_map.player_pos();
  const auto pos_type = raw_map.block(pos.x, pos.y).object;
  if (pos_type == Map::Objects::start_position_n) {
    return std::numbers::pi * 1.5f;
  }
  if (pos_type == Map::Objects::start_position_s) {
    return std::numbers::pi / 2.0f;
  }
  if (pos_type == Map::Objects::start_position_w) {
    return std::numbers::pi;
  }
  return 0.0f;
}

void PlayerState::animate_move_noclip(const std::uint64_t time_elapsed_ms) {
  const auto move_dir = move_direction();
  if (move_dir == 0) {
    return;
  }

  const auto time_factor = time_elapsed_ms / 1000.0f;
  const auto oriented_dir = static_cast<float>(move_dir) * dir();
  pos_ += time_factor * move_speed_ * oriented_dir;
}

void PlayerState::animate_move(const std::uint64_t time_elapsed_ms) {
  const auto move_dir = move_direction();
  if (move_dir == 0) {
    return;
  }

  const auto time_factor = time_elapsed_ms / 1000.0f;
  const auto oriented_dir = static_cast<float>(move_dir) * dir();
  auto new_pos = pos_ + time_factor * move_speed_ * oriented_dir;

  const auto tr = new_pos + half_size_;
  const auto bl = new_pos - half_size_;
  const auto tr_block = glm::ivec2{static_cast<int>(std::floor(tr.x)), static_cast<int>(std::floor(tr.y))};
  const auto bl_block = glm::ivec2{static_cast<int>(std::floor(bl.x)), static_cast<int>(std::floor(bl.y))};

  const auto tr_tr = raw_map_.is_wall(tr_block.x, tr_block.y);
  const auto tr_bl = raw_map_.is_wall(tr_block.x, bl_block.y);
  const auto bl_tr = raw_map_.is_wall(bl_block.x, tr_block.y);
  const auto bl_bl = raw_map_.is_wall(bl_block.x, bl_block.y);

  // Handle corner collisions by pushing the player out along the axis of least penetration
  if (tr_tr && !tr_bl && !bl_tr && !bl_bl) {
    const auto x_penetration = tr.x - std::floor(tr.x);
    const auto y_penetration = tr.y - std::floor(tr.y);
    if (x_penetration < y_penetration) {
      new_pos.x -= x_penetration;
    } else {
      new_pos.y -= y_penetration;
    }
  } else if (!tr_tr && tr_bl && !bl_tr && !bl_bl) {
    const auto x_penetration = tr.x - std::floor(tr.x);
    const auto y_penetration = 1.0f - (bl.y - std::floor(bl.y));
    if (x_penetration < y_penetration) {
      new_pos.x -= x_penetration;
    } else {
      new_pos.y += y_penetration;
    }
  } else if (!tr_tr && !tr_bl && bl_tr && !bl_bl) {
    const auto x_penetration = 1.0f - (bl.x - std::floor(bl.x));
    const auto y_penetration = tr.y - std::floor(tr.y);
    if (x_penetration < y_penetration) {
      new_pos.x += x_penetration;
    } else {
      new_pos.y -= y_penetration;
    }
  } else if (!tr_tr && !tr_bl && !bl_tr && bl_bl) {
    const auto x_penetration = 1.0f - (bl.x - std::floor(bl.x));
    const auto y_penetration = 1.0f - (bl.y - std::floor(bl.y));
    if (x_penetration < y_penetration) {
      new_pos.x += x_penetration;
    } else {
      new_pos.y += y_penetration;
    }
  } else {
    if ((bl_tr && !bl_bl) || (tr_tr && !tr_bl)) {
      new_pos.y -= tr.y - std::floor(tr.y);
    } else if ((bl_bl && !bl_tr) || (tr_bl && !tr_tr)) {
      new_pos.y += 1.0f - (bl.y - std::floor(bl.y));
    }

    if ((tr_bl && !bl_bl) || (tr_tr && !bl_tr)) {
      new_pos.x -= tr.x - std::floor(tr.x);
    } else if ((bl_bl && !tr_bl) || (bl_tr && !tr_tr)) {
      new_pos.x += 1.0f - (bl.x - std::floor(bl.x));
    }
  }

  pos_ = new_pos;
}

void PlayerState::animate_rot(const std::uint64_t time_elapsed_ms) {
  const auto rot_dir = rot_direction();
  if (rot_dir == 0) {
    return;
  }

  const auto time_factor = time_elapsed_ms / 1000.0f;
  orientation_ += rot_dir * time_factor * rot_speed_;
  orientation_ = gp::math::wrap_angle(orientation_);
}

int PlayerState::move_direction() const {
  const auto forward = keyboard_state_->is_down(ScanCode::Up) || keyboard_state_->is_down(ScanCode::W);
  const auto backward = keyboard_state_->is_down(ScanCode::Down) || keyboard_state_->is_down(ScanCode::S);
  if (forward == backward) {
    return 0;
  }
  return forward ? 1 : -1;
}

int PlayerState::rot_direction() const {
  const auto right = keyboard_state_->is_down(ScanCode::Right) || keyboard_state_->is_down(ScanCode::D);
  const auto left = keyboard_state_->is_down(ScanCode::Left) || keyboard_state_->is_down(ScanCode::A);
  if (right == left) {
    return 0;
  }
  return right ? 1 : -1;
}
} // namespace wolf
