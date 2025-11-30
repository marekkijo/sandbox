#pragma once

#include "raycaster.hpp"

#include "wolf_common/player_state.hpp"
#include "wolf_common/raw_map.hpp"

#include <gp/sdl/scene_2d.hpp>

#include <memory>

namespace wolf {
class RaycasterScene : public gp::sdl::Scene2D {
public:
  RaycasterScene(std::unique_ptr<const RawMap> raw_map, const int fov_in_degrees, const int num_rays);

private:
  void loop(const gp::misc::Event &event) override;

  void resize(const int width, const int height);
  void redraw();

  const std::unique_ptr<const RawMap> raw_map_{};
  PlayerState player_state_{*raw_map_};
  Raycaster raycaster_;
  int width_{};
  int height_{};
  std::uint32_t last_timestamp_ms_{0u};
};
} // namespace wolf
