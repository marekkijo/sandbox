#include "wolf_renderer.hpp"

#include "tools/math/math.hpp"

#include <glm/vec3.hpp>

#include <cmath>
#include <limits>
#include <numbers>

namespace wolf {
WolfRenderer::WolfRenderer(tools::sdl::SDLSystem             &sdl_sys,
                           std::shared_ptr<const VectorMap>   vector_map,
                           std::shared_ptr<const PlayerState> player_state,
                           const std::uint32_t                res_w,
                           const std::uint32_t                res_h)
    : Renderer(sdl_sys)
    , vector_map_{vector_map}
    , player_state_{player_state}
    , walls_{res_w}
    , ray_rots_{res_w}
    , res_h_{res_h}
    , wall_heights_{WolfRenderer::res_h() / 2u + WolfRenderer::res_h() % 2u + 1u} {
  const auto fov_step = WolfRenderer::player_state_->fov_rad() / (WolfRenderer::ray_rots().size() - 1u);
  for (std::size_t r_it{0u}; r_it < WolfRenderer::ray_rots().size(); r_it++) {
    const auto angle    = -(WolfRenderer::player_state_->fov_rad() / 2.0f) + fov_step * r_it;
    ray_rots_[r_it].sin = std::sinf(angle);
    ray_rots_[r_it].cos = std::cosf(angle);
  }

  Renderer::resize();
}

void WolfRenderer::redraw() {
  prepare_walls();

  draw_background();
  draw_walls();
}

void WolfRenderer::resize(const int width, const int height) {
  const auto h_part = height / static_cast<double>(res_h());
  for (std::size_t h_it{0u}; h_it < wall_heights().size() - 1; h_it++) {
    wall_heights_[h_it].y = static_cast<int>(std::round(h_part * h_it));
    wall_heights_[h_it].h = height - static_cast<int>(std::round(h_part * h_it * 2));
  }

  const auto w_part = width / static_cast<double>(walls_.size());
  const auto w      = static_cast<int>(std::ceil(w_part));
  for (std::size_t r_it{0u}; r_it < walls_.size(); r_it++) {
    walls_[r_it].rect.x = static_cast<int>(std::floor(w_part * r_it));
    walls_[r_it].rect.w = w;
  }

  ceiling_rect_ = SDL_Rect{0, 0, width, height / 2};
  floor_rect_   = SDL_Rect{0, height / 2, width, height / 2};
}

const std::vector<WolfRenderer::RayRot> &WolfRenderer::ray_rots() const { return ray_rots_; }

std::uint32_t WolfRenderer::res_h() const { return res_h_; }

const std::vector<WolfRenderer::WallHeight> &WolfRenderer::wall_heights() const { return wall_heights_; }

const WolfRenderer::WallHeight &WolfRenderer::wall_height(const float factor) const {
  const auto index =
      std::min(wall_heights().size() - 1u, static_cast<std::size_t>(std::round(factor * wall_heights().size())));
  return wall_heights()[index];
}

const SDL_Rect &WolfRenderer::ceiling_rect() const { return ceiling_rect_; }

const SDL_Rect &WolfRenderer::floor_rect() const { return floor_rect_; }

void WolfRenderer::prepare_walls() {
  const auto cam_cos1 = std::cosf(-std::numbers::pi / 2.0f);
  const auto cam_sin1 = std::sinf(-std::numbers::pi / 2.0f);
  const auto cam_cos2 = std::cosf(std::numbers::pi / 2.0f);
  const auto cam_sin2 = std::sinf(std::numbers::pi / 2.0f);

  const auto dir = player_state_->dir();
  const auto pos = player_state_->pos();

  const auto cam_x1 = (cam_cos1 * dir.x - cam_sin1 * dir.y) + pos.x;
  const auto cam_y1 = (cam_sin1 * dir.x + cam_cos1 * dir.y) + pos.y;
  const auto cam_x2 = (cam_cos2 * dir.x - cam_sin2 * dir.y) + pos.x;
  const auto cam_y2 = (cam_sin2 * dir.x + cam_cos2 * dir.y) + pos.y;

  constexpr auto ray_length = 100.0f;
  for (std::size_t r_it{0u}; r_it < ray_rots().size(); r_it++) {
    const auto ray_cos = ray_rots()[r_it].cos;
    const auto ray_sin = ray_rots()[r_it].sin;
    const auto ray_x   = (ray_cos * dir.x - ray_sin * dir.y) * ray_length + pos.x;
    const auto ray_y   = (ray_sin * dir.x + ray_cos * dir.y) * ray_length + pos.y;

    auto v_index  = std::numeric_limits<std::size_t>::max();
    auto min_dist = ray_length * 2.0f;
    auto min_x    = 0.0f;
    auto min_y    = 0.0f;
    for (std::size_t v_it{0u}; v_it < vector_map_->vectors().size(); v_it++) {
      const auto &v = vector_map_->vectors()[v_it];

      if (!tools::math::do_intersect(pos.x, pos.y, ray_x, ray_y, v.first.x, v.first.y, v.second.x, v.second.y)) {
        continue;
      }
      const auto [crossed, cross_x, cross_y] =
          tools::math::intersection_point(pos.x, pos.y, ray_x, ray_y, v.first.x, v.first.y, v.second.x, v.second.y);
      if (!crossed) { continue; }

      const auto curr_dist = std::sqrtf((cross_x - pos.x) * (cross_x - pos.x) + (cross_y - pos.y) * (cross_y - pos.y));
      if (curr_dist < min_dist) {
        min_dist = curr_dist;
        min_x    = cross_x;
        min_y    = cross_y;
        v_index  = v_it;
      }
    }

    auto cam_dist = 10000.0f;
    if (v_index != std::numeric_limits<std::size_t>::max()) {
      cam_dist  = (cam_x2 - cam_x1) * (cam_y1 - min_y) - (cam_x1 - min_x) * (cam_y2 - cam_y1);
      cam_dist /= std::sqrtf((cam_x2 - cam_x1) * (cam_x2 - cam_x1) + (cam_y2 - cam_y1) * (cam_y2 - cam_y1));
      if (cam_dist < 0.0f) { cam_dist = 0.0f; }
    }

    walls_[r_it].rect.h        = 1.0f / cam_dist * 1024.0f;
    walls_[r_it].rect.y        = (1024.0f - walls_[r_it].rect.h) / 2.0f;
    walls_[r_it].color_index   = v_index == std::numeric_limits<std::size_t>::max() ? 0u : v_index;
    walls_[r_it].shadow_factor = 1.0f - std::min(1.0f, walls_[r_it].rect.h / 1024.0f);
  }
}

void WolfRenderer::draw_background() const {
  SDL_SetRenderDrawColor(r(), 57, 57, 57, 255);
  SDL_RenderFillRect(r(), &ceiling_rect());

  SDL_SetRenderDrawColor(r(), 115, 115, 115, 255);
  SDL_RenderFillRect(r(), &floor_rect());
}

void WolfRenderer::draw_walls() const {
  for (const auto &wall : walls_) {
    const auto color = vector_map_->color(wall.color_index, wall.shadow_factor);
    SDL_SetRenderDrawColor(r(), color.r, color.g, color.b, 255);
    SDL_RenderFillRect(r(), &wall.rect);
  }
}
} // namespace wolf
