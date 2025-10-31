#include "vector_map.hpp"

#include "wolf_common/map_utils.hpp"

#include <cmath>

namespace wolf {
VectorMap::VectorMap(const RawMap &raw_map)
    : width_{static_cast<float>(raw_map.width())}
    , height_{static_cast<float>(raw_map.height())}
    , diagonal_length_{std::sqrtf(width() * width() + height() * height())} {
  generate_vector_map(raw_map);
}

const std::vector<std::pair<glm::vec2, glm::vec2>> &VectorMap::vectors() const { return vectors_; }

const std::vector<glm::uvec3> &VectorMap::colors() const { return colors_; }

glm::uvec3 VectorMap::color(const std::size_t index, const float shadow_factor) const {
  return {std::round(colors()[index].r * shadow_factor),
          std::round(colors()[index].g * shadow_factor),
          std::round(colors()[index].b * shadow_factor)};
}

float VectorMap::width() const { return width_; }

float VectorMap::height() const { return height_; }

float VectorMap::diagonal_length() const { return diagonal_length_; }

void VectorMap::generate_vector_map(const RawMap &raw_map) {
  constexpr auto orientation_shadow_factor{0.625f};

  for (int h = 0; h < raw_map.height(); h++) {
    for (int w = 0; w < raw_map.width(); w++) {
      if (raw_map.is_wall(w, h)) {
        const auto _wall_color = MapUtils::wall_color(raw_map.block(w, h).wall);

        if (raw_map.is_within_bounds(w, h - 1) && !raw_map.is_wall(w, h - 1)) {
          vectors_.emplace_back(glm::vec2{w, h}, glm::vec2{w + 1, h});
          colors_.push_back(_wall_color);
        }
        if (raw_map.is_within_bounds(w, h + 1) && !raw_map.is_wall(w, h + 1)) {
          vectors_.emplace_back(glm::vec2{w + 1, h + 1}, glm::vec2{w, h + 1});
          colors_.push_back(_wall_color);
        }
        if (raw_map.is_within_bounds(w - 1, h) && !raw_map.is_wall(w - 1, h)) {
          vectors_.emplace_back(glm::vec2{w, h + 1}, glm::vec2{w, h});
          colors_.push_back(glm::uvec3{std::round(_wall_color.r * orientation_shadow_factor),
                                       std::round(_wall_color.g * orientation_shadow_factor),
                                       std::round(_wall_color.b * orientation_shadow_factor)});
        }
        if (raw_map.is_within_bounds(w + 1, h) && !raw_map.is_wall(w + 1, h)) {
          vectors_.emplace_back(glm::vec2{w + 1, h}, glm::vec2{w + 1, h + 1});
          colors_.push_back(glm::uvec3{std::round(_wall_color.r * orientation_shadow_factor),
                                       std::round(_wall_color.g * orientation_shadow_factor),
                                       std::round(_wall_color.b * orientation_shadow_factor)});
        }
      }
    }
  }
}
} // namespace wolf
