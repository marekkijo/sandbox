#pragma once

#include "raw_map.hpp"

#include "tools/sdl/sdl.hpp"

#include <memory>
#include <tuple>

namespace wolf {
class PlayerState {
public:
  PlayerState() = delete;
  PlayerState(const PlayerState &) = default;
  PlayerState &operator=(const PlayerState &) = default;
  PlayerState(PlayerState &&) noexcept = default;
  PlayerState &operator=(PlayerState &&) noexcept = default;
  ~PlayerState() = default;

  PlayerState(std::shared_ptr<const RawMap> raw_map, float move_speed = 0.5f, float rot_speed = 0.5f);

  void animate(std::uint32_t time_elapsed_ms);

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
};
} // namespace wolf
