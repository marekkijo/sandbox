#include "projects/wolf/vector_map.hpp"

#include <cmath>
#include <numbers>

namespace wolf {
VectorMap::VectorMap(const std::shared_ptr<const RawMap> raw_map) : raw_map_{raw_map} {
  generate_vector_map();
  diagonal_length_ = std::sqrtf(width() * width() + height() * height());
}

const std::vector<std::pair<glm::vec2, glm::vec2>> &VectorMap::vectors() const { return vectors_; }

const std::vector<glm::uvec3> &VectorMap::colors() const { return colors_; }

glm::uvec3 VectorMap::color(const std::size_t index, const float shadow_factor) const {
  return {std::round(colors()[index].r * shadow_factor),
          std::round(colors()[index].g * shadow_factor),
          std::round(colors()[index].b * shadow_factor)};
}

float VectorMap::width() const { return raw_map_->width(); }

float VectorMap::height() const { return raw_map_->height(); }

float VectorMap::diagonal_length() const { return diagonal_length_; }

void VectorMap::generate_vector_map() {
  constexpr auto orientation_shadow_factor{0.625f};

  for (auto h = std::size_t{0u}; h < raw_map_->height(); h++) {
    for (auto w = std::size_t{0u}; w < raw_map_->width(); w++) {
      if (raw_map_->is_wall(w, h)) {
        const auto _wall_color = wall_color(raw_map_->block(w, h).wall);

        if (!raw_map_->is_wall_on_n(w, h)) {
          vectors_.emplace_back(glm::vec2{static_cast<float>(w), static_cast<float>(h)},
                                glm::vec2{static_cast<float>(w + 1), static_cast<float>(h)});
          colors_.push_back(_wall_color);
        }
        if (!raw_map_->is_wall_on_s(w, h)) {
          vectors_.emplace_back(glm::vec2{static_cast<float>(w + 1), static_cast<float>(h + 1)},
                                glm::vec2{static_cast<float>(w), static_cast<float>(h + 1)});
          colors_.push_back(_wall_color);
        }
        if (!raw_map_->is_wall_on_w(w, h)) {
          vectors_.emplace_back(glm::vec2{static_cast<float>(w), static_cast<float>(h + 1)},
                                glm::vec2{static_cast<float>(w), static_cast<float>(h)});
          colors_.push_back(glm::uvec3{std::round(_wall_color.r * orientation_shadow_factor),
                                       std::round(_wall_color.g * orientation_shadow_factor),
                                       std::round(_wall_color.b * orientation_shadow_factor)});
        }
        if (!raw_map_->is_wall_on_e(w, h)) {
          vectors_.emplace_back(glm::vec2{static_cast<float>(w + 1), static_cast<float>(h)},
                                glm::vec2{static_cast<float>(w + 1), static_cast<float>(h + 1)});
          colors_.push_back(glm::uvec3{std::round(_wall_color.r * orientation_shadow_factor),
                                       std::round(_wall_color.g * orientation_shadow_factor),
                                       std::round(_wall_color.b * orientation_shadow_factor)});
        }
      }
    }
  }
}

glm::uvec3 VectorMap::wall_color(const Map::Walls wall) {
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
  case Map::Walls::fake_elevator: return {160, 160, 160};

  case Map::Walls::dirty_brick_1:
  case Map::Walls::dirty_brick_2: return {160, 160, 90};

  case Map::Walls::cell:
  case Map::Walls::cell_skeleton:
  case Map::Walls::blue_brick_1:
  case Map::Walls::blue_brick_2:
  case Map::Walls::blue_brick_sign:
  case Map::Walls::blue_wall:
  case Map::Walls::blue_wall_swastika:
  case Map::Walls::blue_wall_skull: return {64, 80, 224};

  case Map::Walls::wood:
  case Map::Walls::wood_eagle:
  case Map::Walls::wood_hitler:
  case Map::Walls::wood_iron_cross:
  case Map::Walls::wood_panel: return {106, 70, 34};

  case Map::Walls::entrance_to_level: return {0, 154, 56};

  case Map::Walls::steel:
  case Map::Walls::steel_sign:
  case Map::Walls::fake_door:
  case Map::Walls::door_excavation: return {24, 148, 148};

  case Map::Walls::landscape: return {124, 246, 246};

  case Map::Walls::red_brick:
  case Map::Walls::red_brick_swastika:
  case Map::Walls::red_brick_flag:
  case Map::Walls::multicolor_brick: return {160, 0, 0};

  case Map::Walls::purple:
  case Map::Walls::purple_blood: return {160, 0, 160};

  case Map::Walls::brown_weave:
  case Map::Walls::brown_weave_blood_1:
  case Map::Walls::brown_weave_blood_2:
  case Map::Walls::brown_weave_blood_3:
  case Map::Walls::brown_stone_1:
  case Map::Walls::brown_stone_2:
  case Map::Walls::brown_marble_1:
  case Map::Walls::brown_marble_2:
  case Map::Walls::brown_marble_flag: return {220, 162, 128};

  case Map::Walls::stained_glass: return {252, 248, 92};

  default: return {255, 255, 255};
  }
}
} // namespace wolf
