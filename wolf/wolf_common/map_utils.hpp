#pragma once

#include "wolf_common/wolf_map_info.hpp"

#include <glm/vec3.hpp>

namespace wolf::MapUtils {

// Brightness factor applied to E/W-facing walls to give the illusion of directional lighting.
// Used by both the map overlay (VectorMap) and the solid-color raycaster path.
inline constexpr auto orientation_shadow_factor = 0.625f;

glm::uvec3 wall_color(const Map::Walls wall);
} // namespace wolf::MapUtils
