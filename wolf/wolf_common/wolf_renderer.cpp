#include "wolf_renderer.hpp"

#include <gp/math/math.hpp>

#include <glm/vec3.hpp>

#include <cmath>

namespace wolf {
WolfRenderer::WolfRenderer(gp::sdl::System &sdl_sys,
                           const std::shared_ptr<const VectorMap> vector_map,
                           const std::shared_ptr<const PlayerState> player_state,
                           const std::uint32_t rays)
    : Renderer(sdl_sys)
    , vector_map_{vector_map}
    , player_state_{player_state}
    , walls_(rays)
    , ray_rots_(rays) {
  const auto fov_step = player_state_->fov_rad() / (ray_rots_.size() - 1u);
  for (auto r_it = std::size_t{0u}; r_it < ray_rots_.size(); r_it++) {
    const auto angle = -(player_state_->fov_rad() / 2.0f) + fov_step * r_it;
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
  width_ = width;
  height_ = height;

  const auto w_part = width_ / static_cast<double>(walls_.size());
  const auto w = static_cast<int>(std::ceil(w_part));
  for (auto w_it = std::size_t{0u}; w_it < walls_.size(); w_it++) {
    walls_[w_it].rect.x = static_cast<int>(std::floor(w_part * w_it));
    walls_[w_it].rect.w = w;
  }
}

void WolfRenderer::draw_background() const {
  SDL_SetRenderDrawColor(r(), 57, 57, 57, 255);
  const auto ceiling_rect = SDL_Rect{0, 0, width_, height_ / 2};
  SDL_RenderFillRect(r(), &ceiling_rect);

  SDL_SetRenderDrawColor(r(), 115, 115, 115, 255);
  const auto floor_rect = SDL_Rect{0, height_ / 2, width_, height_ / 2};
  SDL_RenderFillRect(r(), &floor_rect);
}

void WolfRenderer::draw_walls() const {
  for (const auto &wall : walls_) {
    const auto color = vector_map_->color(wall.color_index, wall.shadow_factor);
    SDL_SetRenderDrawColor(r(), color.r, color.g, color.b, 255);
    SDL_RenderFillRect(r(), &wall.rect);
  }
}
} // namespace wolf
