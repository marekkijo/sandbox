#pragma once

#include "raw_map.hpp"

#include "tools/sdl/sdl_keyboard_state.hpp"

#include <memory>
#include <tuple>

namespace wolf {
class PlayerState {
public:
  PlayerState(const PlayerState &) = default;
  PlayerState &operator=(const PlayerState &) = default;
  PlayerState(PlayerState &&) noexcept = default;
  PlayerState &operator=(PlayerState &&) noexcept = default;
  ~PlayerState() = default;

  PlayerState(std::shared_ptr<const RawMap> raw_map, float fov_deg = 45.0f, float move_speed = 1.0f,
              float rot_speed = 1.0f);

  void animate(std::uint32_t time_elapsed_ms);

  [[nodiscard]] float fov_deg() const;
  [[nodiscard]] float fov_rad() const;
  [[nodiscard]] float pos_x() const;
  [[nodiscard]] float pos_y() const;
  [[nodiscard]] std::tuple<float, float> pos() const;
  [[nodiscard]] float orientation() const;
  [[nodiscard]] float dir_x() const;
  [[nodiscard]] float dir_y() const;
  [[nodiscard]] std::tuple<float, float> dir() const;

private:
  const std::shared_ptr<const RawMap> raw_map_{};
  const tools::sdl::SDLKeyboardState keyboard_state_{};
  const float fov_rad_{};
  const float move_speed_{};
  const float rot_speed_{};
  float pos_x_{};
  float pos_y_{};
  float orientation_{};
  float dir_x_{};
  float dir_y_{};

  [[nodiscard]] const RawMap &raw_map() const;
  [[nodiscard]] const tools::sdl::SDLKeyboardState &keyboard_state() const;
  [[nodiscard]] float move_speed() const;
  [[nodiscard]] float rot_speed() const;
  void animate_move(std::uint32_t time_elapsed_ms);
  void animate_rot(std::uint32_t time_elapsed_ms);
  void update_dir();
};
} // namespace wolf
