#pragma once

#include "wolf_common/player_state.hpp"
#include "wolf_common/raw_map.hpp"

namespace wolf {
struct RayInfo {
  float dist{};
};

class Raycaster {
public:
  Raycaster(const RawMap &raw_map, const PlayerState &player_state, int fov_in_degrees, int num_rays);

  int num_rays() const;
  const std::vector<RayInfo> &rays() const;
  void cast_rays();

private:
  static constexpr auto line_length_ = 100.0f;

  const RawMap &raw_map_;
  const PlayerState &player_state_;
  float fov_;
  std::vector<RayInfo> rays_{};

  glm::vec2 find_collision(const float ray_angle) const;
  float project_to_camera_plane(const glm::vec2 &pos) const;
};
} // namespace wolf
