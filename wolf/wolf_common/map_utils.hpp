#pragma once

#include "wolf_common/wolf_map_info.hpp"

#include <glm/vec3.hpp>

namespace wolf::MapUtils {
glm::uvec3 wall_color(const Map::Walls wall);
} // namespace wolf::MapUtils
