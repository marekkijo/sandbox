#pragma once

#include "wolf_common/raw_map.hpp"

#include <gp/misc/keyboard_state.hpp>

#include <glm/vec2.hpp>

#include <memory>

namespace wolf {
class PlayerState {
public:
  explicit PlayerState(const RawMap &raw_map, const float move_speed = 1.0f, const float rot_speed = 1.0f);

  PlayerState(const PlayerState &) = default;
  PlayerState &operator=(const PlayerState &) = default;
  PlayerState(PlayerState &&) noexcept = default;
  PlayerState &operator=(PlayerState &&) noexcept = default;
  ~PlayerState() = default;

  void set_keyboard_state(std::shared_ptr<gp::misc::KeyboardState> keyboard_state);
  void animate(const std::uint32_t time_elapsed_ms);

  [[nodiscard]] float orientation() const;
  [[nodiscard]] const glm::vec2 &pos() const;
  [[nodiscard]] const glm::vec2 &dir() const;

private:
  const float move_speed_{};
  const float rot_speed_{};
  float orientation_{};
  glm::vec2 pos_{};
  glm::vec2 dir_{};

  std::shared_ptr<gp::misc::KeyboardState> keyboard_state_{};

  [[nodiscard]] float deduce_orientation(const RawMap &raw_map) const;
  void animate_move(const std::uint32_t time_elapsed_ms);
  void animate_rot(const std::uint32_t time_elapsed_ms);
  void update_dir();
};
} // namespace wolf
