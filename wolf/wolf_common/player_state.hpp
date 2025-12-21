#pragma once

#include "wolf_common/raw_map.hpp"

#include <gp/misc/keyboard_state.hpp>

#include <SDL3/SDL.h>
#include <glm/vec2.hpp>

#include <memory>

namespace wolf {
class PlayerState {
public:
  explicit PlayerState(const RawMap &raw_map, const float move_speed = 3.0f, const float rot_speed = 1.5f);

  void set_keyboard_state(std::shared_ptr<gp::misc::KeyboardState> keyboard_state);
  void animate(const std::uint64_t time_elapsed_ms);

  float orientation() const;
  glm::vec2 pos() const;
  glm::ivec2 block_pos() const;
  glm::vec2 dir() const;

private:
  static constexpr auto player_size_ = 0.75f;

  const RawMap &raw_map_;
  const float move_speed_{};
  const float rot_speed_{};
  float orientation_{};
  glm::vec2 pos_{};

  std::shared_ptr<gp::misc::KeyboardState> dummy_keyboard_state_{std::make_shared<gp::misc::KeyboardState>()};
  std::shared_ptr<gp::misc::KeyboardState> keyboard_state_{dummy_keyboard_state_};

  float deduce_orientation(const RawMap &raw_map) const;
  void animate_move_noclip(const std::uint64_t time_elapsed_ms);
  void animate_move(const std::uint64_t time_elapsed_ms);
  void animate_rot(const std::uint64_t time_elapsed_ms);

  int move_direction() const;
  int rot_direction() const;

  float resolved_x_collision(const float delta_x) const;
  float resolved_y_collision(const float delta_y) const;

  glm::vec2 get_move_delta(const std::uint64_t time_elapsed_ms) const;
  SDL_FRect get_player_rect() const;
};
} // namespace wolf
