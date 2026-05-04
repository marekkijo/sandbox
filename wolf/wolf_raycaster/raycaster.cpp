#include "raycaster.hpp"

#include <glm/ext/vector_int2.hpp>
#include <glm/geometric.hpp>
#include <gp/utils/utils.hpp>

#include <algorithm>
#include <cmath>
#include <numbers>

namespace wolf {
Raycaster::Raycaster(const RawMap &raw_map, const PlayerState &player_state, int fov_in_degrees, int num_rays)
    : raw_map_(raw_map)
    , player_state_(player_state)
    , fov_(fov_in_degrees * std::numbers::pi / 180.0f)
    , rays_(num_rays) {}

int Raycaster::num_rays() const { return static_cast<int>(rays_.size()); }

const std::vector<RayInfo> &Raycaster::rays() const { return rays_; }

void Raycaster::cast_rays() {
  const auto ray_angle_step = fov_ / static_cast<float>(rays_.size());
  const auto scan_start = player_state_.orientation() - (fov_ / 2.0f);
  for (std::size_t r_it = 0; r_it < rays_.size(); r_it++) {
    const auto ray_angle = scan_start + ray_angle_step * static_cast<float>(r_it);
    auto collision = find_collision(ray_angle);
    rays_[r_it].dist = project_to_camera_plane(collision.pos);
    rays_[r_it].wall = collision.wall;
    rays_[r_it].x_facing = collision.x_facing;
    rays_[r_it].tex_u = collision.tex_u;
  }
}

Raycaster::CollisionResult Raycaster::find_collision(const float ray_angle) const {
  const auto max_depth = std::max(raw_map_.width(), raw_map_.height());
  const auto ray_dir = gp::utils::orientation_to_dir(ray_angle);
  const auto player_block_pos = player_state_.block_pos();
  const auto search_dir = glm::ivec2{
      ray_dir.x > 0.0f ? 1 : -1,
      ray_dir.y > 0.0f ? 1 : -1,
  };
  const auto player_pos = player_state_.pos();
  const auto scan_end = player_pos + ray_dir * line_length_;
  auto result = scan_end;
  auto result_wall = Map::Walls::nothing;
  auto result_x_facing = false;
  auto result_tex_u = 0.0f;
  auto found = false;
  auto min_dist = line_length_;

  const auto intersection_check =
      [this, &player_pos, &scan_end, &ray_dir, &min_dist, &result, &result_wall, &result_x_facing, &result_tex_u](
          const glm::ivec2 &wall_pos) -> bool {
    if (!raw_map_.is_wall(wall_pos)) {
      return false;
    }

    const auto wall_rect = gp::utils::rect_at(wall_pos);
    auto line_start = player_pos;
    auto line_end = scan_end;
    if (!SDL_GetRectAndLineIntersectionFloat(&wall_rect, &line_start.x, &line_start.y, &line_end.x, &line_end.y)) {
      return false;
    }

    const auto dist = glm::distance(player_pos, line_start);
    if (dist < min_dist) {
      min_dist = dist;
      result = line_start;
      result_wall = raw_map_.block(wall_pos).wall;
      // Determine face orientation: compare closeness to x-boundary vs y-boundary.
      // x-facing = hit the left or right face of the block (x is constant).
      const auto dx = std::min(std::abs(line_start.x - static_cast<float>(wall_pos.x)),
                               std::abs(line_start.x - static_cast<float>(wall_pos.x + 1)));
      const auto dy = std::min(std::abs(line_start.y - static_cast<float>(wall_pos.y)),
                               std::abs(line_start.y - static_cast<float>(wall_pos.y + 1)));
      result_x_facing = dx < dy;
      // Texture u-coordinate: fractional part of the component that varies along the face,
      // flipped based on ray direction so the texture reads consistently left-to-right from
      // the viewer's perspective regardless of which face is hit.
      const auto raw_u =
          result_x_facing ? (line_start.y - std::floor(line_start.y)) : (line_start.x - std::floor(line_start.x));
      const auto flip = (result_x_facing && ray_dir.x < 0.0f) || (!result_x_facing && ray_dir.y < 0.0f);
      result_tex_u = flip ? (1.0f - raw_u) : raw_u;
    }

    return true;
  };

  for (auto depth = 1; depth < max_depth; ++depth) {
    const auto max_x = player_block_pos.x + search_dir.x * depth;
    const auto min_x = player_block_pos.x;
    const auto max_y = player_block_pos.y + search_dir.y * depth;
    const auto min_y = player_block_pos.y;

    if (!raw_map_.is_within_bounds(max_x, min_y) && !raw_map_.is_within_bounds(min_x, max_y)) {
      break;
    }

    for (auto y_depth = 0; y_depth <= depth; ++y_depth) {
      const auto curr_y = min_y + search_dir.y * y_depth;
      found |= intersection_check(glm::ivec2{max_x, curr_y});
    }

    for (auto x_depth = depth - 1; x_depth >= 0; --x_depth) {
      const auto curr_x = min_x + search_dir.x * x_depth;
      found |= intersection_check(glm::ivec2{curr_x, max_y});
    }

    if (found) {
      break;
    }
  }

  return {result, result_wall, result_x_facing, result_tex_u};
}

float Raycaster::project_to_camera_plane(const glm::vec2 &pos) const {
  const auto to_player = pos - player_state_.pos();
  const auto N = player_state_.dir();
  const auto distance = glm::dot(to_player, N);
  return std::abs(distance);
}
} // namespace wolf
