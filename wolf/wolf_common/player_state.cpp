#include "player_state.hpp"

#include "wolf_common/wolf_map_info.hpp"

#include <gp/math/math.hpp>
#include <gp/utils/utils.hpp>

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
  if (keyboard_state == nullptr) {
    keyboard_state_ = dummy_keyboard_state_;
    return;
  }
  keyboard_state_ = std::move(keyboard_state);
}

void PlayerState::animate(const std::uint64_t time_elapsed_ms) {
  animate_move(time_elapsed_ms);
  animate_rot(time_elapsed_ms);
}

float PlayerState::orientation() const { return orientation_; }

glm::vec2 PlayerState::pos() const { return pos_; }

glm::ivec2 PlayerState::block_pos() const {
  auto block_pos = glm::ivec2{static_cast<int>(std::floor(pos_.x)), static_cast<int>(std::floor(pos_.y))};
  return glm::min(block_pos, raw_map_.bounds());
}

glm::vec2 PlayerState::dir() const { return gp::utils::orientation_to_dir(orientation_); }

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
  const auto move_delta = get_move_delta(time_elapsed_ms);
  pos_ += move_delta;
}

void PlayerState::animate_move(const std::uint64_t time_elapsed_ms) {
  const auto move_delta = get_move_delta(time_elapsed_ms);
  if (move_delta == glm::vec2{}) {
    return;
  }

  const auto new_delta_x = resolved_x_collision(move_delta.x);
  const auto new_delta_y = resolved_y_collision(move_delta.y);
  pos_ += glm::vec2{new_delta_x, new_delta_y};
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

float PlayerState::resolved_x_collision(const float delta_x) const {
  const auto moving_right = delta_x > 0.0f;
  const auto player_block_pos = block_pos();
  auto check_block_pos = moving_right ? glm::ivec2{player_block_pos.x + 1, player_block_pos.y - 1}
                                      : glm::ivec2{player_block_pos.x - 1, player_block_pos.y - 1};

  auto check_rect = get_player_rect();
  check_rect.x += delta_x;
  do {
    if (raw_map_.is_wall(check_block_pos)) {
      const auto wall_rect = gp::utils::rect_at(check_block_pos);
      SDL_FRect intersection;
      if (SDL_GetRectIntersectionFloat(&check_rect, &wall_rect, &intersection)) {
        intersection.w += 0.001f; // Prevent sticking to wall due to float precision
        return delta_x - (moving_right ? intersection.w : -intersection.w);
      }
    }
  } while (++check_block_pos.y <= player_block_pos.y + 1);

  return delta_x;
}

float PlayerState::resolved_y_collision(const float delta_y) const {
  const auto moving_down = delta_y > 0.0f;
  const auto player_block_pos = block_pos();
  auto check_block_pos = moving_down ? glm::ivec2{player_block_pos.x - 1, player_block_pos.y + 1}
                                     : glm::ivec2{player_block_pos.x - 1, player_block_pos.y - 1};
  auto check_rect = get_player_rect();
  check_rect.y += delta_y;
  do {
    if (raw_map_.is_wall(check_block_pos)) {
      const auto wall_rect = gp::utils::rect_at(check_block_pos);
      SDL_FRect intersection;
      if (SDL_GetRectIntersectionFloat(&check_rect, &wall_rect, &intersection)) {
        intersection.h += 0.001f; // Prevent sticking to wall due to float precision
        return delta_y - (moving_down ? intersection.h : -intersection.h);
      }
    }
  } while (++check_block_pos.x <= player_block_pos.x + 1);

  return delta_y;
}

glm::vec2 PlayerState::get_move_delta(const std::uint64_t time_elapsed_ms) const {
  const auto move_dir = move_direction();
  if (move_dir == 0) {
    return glm::vec2{};
  }

  const auto time_factor = time_elapsed_ms / 1000.0f;
  const auto oriented_dir = static_cast<float>(move_dir) * dir();
  return time_factor * move_speed_ * oriented_dir;
}

SDL_FRect PlayerState::get_player_rect() const {
  return gp::utils::rect_at(pos_ - glm::vec2{player_size_ / 2.0f}, player_size_);
}
} // namespace wolf
