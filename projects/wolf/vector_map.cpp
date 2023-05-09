#include "vector_map.hpp"

namespace wolf {
VectorMap::VectorMap(const RawMap &raw_map) {
  for (std::size_t h = 0; h < raw_map.height(); h++) {
    for (std::size_t w = 0; w < raw_map.width(); w++) {
      if (raw_map.is_wall(w, h)) {
        const auto &curr_color = raw_map.block(w, h);

        // clang-format off
        if (!raw_map.is_wall_on_n(w, h)) {
          vectors_.push_back({static_cast<float>(w),
                              static_cast<float>(h),
                              static_cast<float>(w + 1),
                              static_cast<float>(h)});
          colors_.push_back(curr_color);
        }
        if (!raw_map.is_wall_on_s(w, h)) {
          vectors_.push_back({static_cast<float>(w + 1),
                              static_cast<float>(h + 1),
                              static_cast<float>(w),
                              static_cast<float>(h + 1)});
          colors_.push_back(curr_color);
        }
        if (!raw_map.is_wall_on_w(w, h)) {
          vectors_.push_back({static_cast<float>(w),
                              static_cast<float>(h + 1),
                              static_cast<float>(w),
                              static_cast<float>(h)});
          colors_.push_back(curr_color);
        }
        if (!raw_map.is_wall_on_e(w, h)) {
          vectors_.push_back({static_cast<float>(w + 1),
                              static_cast<float>(h),
                              static_cast<float>(w + 1),
                              static_cast<float>(h + 1)});
          colors_.push_back(curr_color);
        }
        // clang-format on
      }
    }
  }
}

const std::vector<std::array<float, 4>> &VectorMap::vectors() const { return vectors_; }

const std::vector<char> &VectorMap::colors() const { return colors_; }
} // namespace wolf
