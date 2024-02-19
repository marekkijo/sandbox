#pragma once

#include "raw_map.hpp"

#include <gp/sdl/keyboard_state.hpp>

#include <glm/vec2.hpp>

#include <memory>

namespace wolf {
class PlayerState {
public:
  PlayerState(const PlayerState &) = default;
  PlayerState &operator=(const PlayerState &) = default;
  PlayerState(PlayerState &&) noexcept = default;
  PlayerState &operator=(PlayerState &&) noexcept = default;
  ~PlayerState() = default;

  PlayerState(const std::shared_ptr<const RawMap> raw_map,
              const float fov_deg = 45.0f,
              const float move_speed = 1.0f,
              const float rot_speed = 1.0f);

  void animate(const std::uint32_t time_elapsed_ms);

  [[nodiscard]] float fov_rad() const;
  [[nodiscard]] float orientation() const;
  [[nodiscard]] const glm::vec2 &pos() const;
  [[nodiscard]] const glm::vec2 &dir() const;

private:
  const std::shared_ptr<const RawMap> raw_map_{};
  const gp::sdl::KeyboardState keyboard_state_{};
  const float fov_rad_{};
  const float move_speed_{};
  const float rot_speed_{};

  float orientation_{};
  glm::vec2 pos_{};
  glm::vec2 dir_{};

  [[nodiscard]] float deduce_orientation() const;
  void animate_move(const std::uint32_t time_elapsed_ms);
  void animate_rot(const std::uint32_t time_elapsed_ms);
  void update_dir();
};
} // namespace wolf
