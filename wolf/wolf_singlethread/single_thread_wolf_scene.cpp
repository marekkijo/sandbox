#include "single_thread_wolf_scene.hpp"

#include <gp/math/math.hpp>

#include <cmath>
#include <numbers>

namespace wolf {
SingleThreadWolfScene::SingleThreadWolfScene(const RawMap &raw_map,
                                             const std::uint32_t fov_in_degrees,
                                             const std::uint32_t number_of_rays)
    : WolfScene{raw_map, fov_in_degrees, number_of_rays} {}

void SingleThreadWolfScene::prepare_walls() {
  constexpr auto ray_length{100.0f};

  const auto cam_cos1 = std::cosf(-std::numbers::pi_v<float> / 2.0f);
  const auto cam_sin1 = std::sinf(-std::numbers::pi_v<float> / 2.0f);
  const auto cam_cos2 = std::cosf(std::numbers::pi_v<float> / 2.0f);
  const auto cam_sin2 = std::sinf(std::numbers::pi_v<float> / 2.0f);

  const auto dir = player_state_->dir();
  const auto pos = player_state_->pos();

  const auto cam_x1 = (cam_cos1 * dir.x - cam_sin1 * dir.y) + pos.x;
  const auto cam_y1 = (cam_sin1 * dir.x + cam_cos1 * dir.y) + pos.y;
  const auto cam_x2 = (cam_cos2 * dir.x - cam_sin2 * dir.y) + pos.x;
  const auto cam_y2 = (cam_sin2 * dir.x + cam_cos2 * dir.y) + pos.y;

  for (auto r_it = std::size_t{0u}; r_it < ray_rots_.size(); r_it++) {
    const auto ray_cos = ray_rots_[r_it].cos;
    const auto ray_sin = ray_rots_[r_it].sin;
    const auto ray_x = (ray_cos * dir.x - ray_sin * dir.y) * ray_length + pos.x;
    const auto ray_y = (ray_sin * dir.x + ray_cos * dir.y) * ray_length + pos.y;

    auto v_index = std::numeric_limits<std::size_t>::max();
    auto min_dist = ray_length * 2.0f;
    auto min_x = 0.0f;
    auto min_y = 0.0f;
    for (auto v_it = std::size_t{0u}; v_it < vector_map_->vectors().size(); v_it++) {
      const auto &v = vector_map_->vectors()[v_it];

      if (!gp::math::do_intersect(pos.x, pos.y, ray_x, ray_y, v.first.x, v.first.y, v.second.x, v.second.y)) {
        continue;
      }
      const auto [crossed, cross_x, cross_y] =
          gp::math::intersection_point(pos.x, pos.y, ray_x, ray_y, v.first.x, v.first.y, v.second.x, v.second.y);
      if (!crossed) {
        continue;
      }

      const auto curr_dist = std::sqrtf((cross_x - pos.x) * (cross_x - pos.x) + (cross_y - pos.y) * (cross_y - pos.y));
      if (curr_dist < min_dist) {
        min_dist = curr_dist;
        min_x = cross_x;
        min_y = cross_y;
        v_index = v_it;
      }
    }

    auto cam_dist = std::numeric_limits<float>::max();
    if (v_index != std::numeric_limits<std::size_t>::max()) {
      cam_dist = (cam_x2 - cam_x1) * (cam_y1 - min_y) - (cam_x1 - min_x) * (cam_y2 - cam_y1);
      cam_dist /= std::sqrtf((cam_x2 - cam_x1) * (cam_x2 - cam_x1) + (cam_y2 - cam_y1) * (cam_y2 - cam_y1));
      if (cam_dist < 0.0f) {
        cam_dist = 0.0f;
      }
    }

    walls_[r_it].rect.h = static_cast<int>(1.0f / cam_dist * height_);
    walls_[r_it].rect.y = static_cast<int>((height_ - walls_[r_it].rect.h) / 2.0f);
    walls_[r_it].color_index = v_index == std::numeric_limits<std::size_t>::max() ? 0u : v_index;

    const int num_steps = 8;
    const float step_size = 0.75f / num_steps;
    const float shadow_factor = 1.0f - std::min(0.75f, static_cast<float>(walls_[r_it].rect.h) / height_);

    walls_[r_it].shadow_factor = static_cast<float>(static_cast<int>(shadow_factor / step_size)) * step_size;
  }
}
} // namespace wolf
