#include "vector_map.hpp"

#include <cmath>
#include <numbers>

namespace wolf {
VectorMap::VectorMap(std::shared_ptr<const RawMap> raw_map) : raw_map_{raw_map} {
  generate_vector_map();
  diagonal_length_ = std::sqrtf(width() * width() + height() * height());
}

const std::vector<std::pair<glm::vec2, glm::vec2>> &VectorMap::vectors() const { return vectors_; }

glm::uvec3 VectorMap::color(const std::size_t index, const float shadow_factor) const {
  return {static_cast<std::uint32_t>(65.0f * shadow_factor),
          static_cast<std::uint32_t>(80.0f * shadow_factor),
          120 + static_cast<std::uint32_t>(100.0f * shadow_factor)};
}

void VectorMap::generate_vector_map() {
  for (std::size_t h{0u}; h < raw_map_->height(); h++) {
    for (std::size_t w{0u}; w < raw_map_->width(); w++) {
      if (raw_map_->is_wall(w, h)) {
        const auto &curr_color = raw_map_->block(w, h);

        if (!raw_map_->is_wall_on_n(w, h)) {
          vectors_.emplace_back(glm::vec2{static_cast<float>(w), static_cast<float>(h)},
                                glm::vec2{static_cast<float>(w + 1), static_cast<float>(h)});
          colors_.push_back(curr_color);
        }
        if (!raw_map_->is_wall_on_s(w, h)) {
          vectors_.emplace_back(glm::vec2{static_cast<float>(w + 1), static_cast<float>(h + 1)},
                                glm::vec2{static_cast<float>(w), static_cast<float>(h + 1)});
          colors_.push_back(curr_color);
        }
        if (!raw_map_->is_wall_on_w(w, h)) {
          vectors_.emplace_back(glm::vec2{static_cast<float>(w), static_cast<float>(h + 1)},
                                glm::vec2{static_cast<float>(w), static_cast<float>(h)});
          colors_.push_back(curr_color);
        }
        if (!raw_map_->is_wall_on_e(w, h)) {
          vectors_.emplace_back(glm::vec2{static_cast<float>(w + 1), static_cast<float>(h)},
                                glm::vec2{static_cast<float>(w + 1), static_cast<float>(h + 1)});
          colors_.push_back(curr_color);
        }
      }
    }
  }
}

float VectorMap::width() const { return raw_map_->width(); }

float VectorMap::height() const { return raw_map_->height(); }

float VectorMap::diagonal_length() const { return diagonal_length_; }
} // namespace wolf
