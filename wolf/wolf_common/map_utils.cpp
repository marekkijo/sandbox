#include "map_utils.hpp"

namespace wolf::MapUtils {
glm::uvec3 wall_color(const Map::Walls wall) {
  switch (wall) {
  case Map::Walls::grey_brick_1:
  case Map::Walls::grey_brick_2:
  case Map::Walls::grey_brick_3:
  case Map::Walls::grey_brick_flag:
  case Map::Walls::grey_brick_hitler:
  case Map::Walls::grey_brick_eagle:
  case Map::Walls::grey_brick_sign:
  case Map::Walls::grey_wall_1:
  case Map::Walls::grey_wall_2:
  case Map::Walls::grey_wall_hitler:
  case Map::Walls::grey_wall_map:
  case Map::Walls::grey_wall_vent:
  case Map::Walls::fake_locked_door:
  case Map::Walls::elevator_wall:
  case Map::Walls::elevator:
  case Map::Walls::fake_elevator:
    return {160, 160, 160};

  case Map::Walls::dirty_brick_1:
  case Map::Walls::dirty_brick_2:
    return {160, 160, 90};

  case Map::Walls::cell:
  case Map::Walls::cell_skeleton:
  case Map::Walls::blue_brick_1:
  case Map::Walls::blue_brick_2:
  case Map::Walls::blue_brick_sign:
  case Map::Walls::blue_wall:
  case Map::Walls::blue_wall_swastika:
  case Map::Walls::blue_wall_skull:
    return {64, 80, 224};

  case Map::Walls::wood:
  case Map::Walls::wood_eagle:
  case Map::Walls::wood_hitler:
  case Map::Walls::wood_iron_cross:
  case Map::Walls::wood_panel:
    return {106, 70, 34};

  case Map::Walls::entrance_to_level:
    return {0, 154, 56};

  case Map::Walls::steel:
  case Map::Walls::steel_sign:
  case Map::Walls::fake_door:
  case Map::Walls::door_excavation:
    return {24, 148, 148};

  case Map::Walls::landscape:
    return {124, 246, 246};

  case Map::Walls::red_brick:
  case Map::Walls::red_brick_swastika:
  case Map::Walls::red_brick_flag:
  case Map::Walls::multicolor_brick:
    return {160, 0, 0};

  case Map::Walls::purple:
  case Map::Walls::purple_blood:
    return {160, 0, 160};

  case Map::Walls::brown_weave:
  case Map::Walls::brown_weave_blood_1:
  case Map::Walls::brown_weave_blood_2:
  case Map::Walls::brown_weave_blood_3:
  case Map::Walls::brown_stone_1:
  case Map::Walls::brown_stone_2:
  case Map::Walls::brown_marble_1:
  case Map::Walls::brown_marble_2:
  case Map::Walls::brown_marble_flag:
    return {220, 162, 128};

  case Map::Walls::stained_glass:
    return {252, 248, 92};

  default:
    return {255, 255, 255};
  }
}
} // namespace wolf::MapUtils
