#include "projects/wolf/map_renderer.hpp"

#include <glm/ext/matrix_transform.hpp>

#include <numbers>

namespace wolf {
MapRenderer::MapRenderer(tools::sdl::SDLSystem &sdl_sys,
                         const std::shared_ptr<const VectorMap> vector_map,
                         const std::shared_ptr<const PlayerState> player_state,
                         const bool player_oriented)
    : Renderer(sdl_sys)
    , vector_map_{vector_map}
    , player_state_{player_state}
    , player_oriented_{player_oriented} {
  Renderer::resize();
}

void MapRenderer::redraw() {
  if (player_oriented_) {
    redraw_player_oriented();
  } else {
    redraw_map_oriented();
  }
}

void MapRenderer::resize(const int width, const int height) {
  scale_ = std::min(width, height) / vector_map_->diagonal_length();
  screen_center_translation_ = glm::vec3{static_cast<float>(width) / 2.0f, static_cast<float>(height) / 2.0f, 0.0f};
}

void MapRenderer::redraw_player_oriented() const {
  // to the center of the screen
  auto map_mat = glm::translate(glm::mat4(1.0f), screen_center_translation_);
  // orient to north
  map_mat = glm::rotate(map_mat, -static_cast<float>(std::numbers::pi / 2.0f), glm::vec3(0.0f, 0.0f, 1.0f));
  // scale map
  map_mat = glm::scale(map_mat, glm::vec3(scale_));
  // player rotation
  map_mat = glm::rotate(map_mat, -player_state_->orientation(), glm::vec3(0.0f, 0.0f, 1.0f));
  // player translation
  map_mat = glm::translate(map_mat, glm::vec3(-player_state_->pos().x, -player_state_->pos().y, 0.0f));

  draw_map(map_mat);

  // to the center of the screen
  auto player_mat = glm::translate(glm::mat4(1.0f), screen_center_translation_);
  // orient to north
  player_mat = glm::rotate(player_mat, -static_cast<float>(std::numbers::pi / 2.0f), glm::vec3(0.0f, 0.0f, 1.0f));
  // scale map
  player_mat = glm::scale(player_mat, glm::vec3(scale_));

  draw_player(player_mat);
}

void MapRenderer::redraw_map_oriented() const {
  // to the center of the screen
  auto map_mat = glm::translate(glm::mat4(1.0f), screen_center_translation_);
  // scale map
  map_mat = glm::scale(map_mat, glm::vec3(scale_));
  // to the center of the map
  map_mat = glm::translate(map_mat, glm::vec3(-vector_map_->width() / 2.0f, -vector_map_->height() / 2.0f, 0.0f));

  draw_map(map_mat);

  // to the center of the screen
  auto player_mat = glm::translate(glm::mat4(1.0f), screen_center_translation_);
  // scale map
  player_mat = glm::scale(player_mat, glm::vec3(scale_));
  // to the center of the map
  player_mat = glm::translate(player_mat, glm::vec3(-vector_map_->width() / 2.0f, -vector_map_->height() / 2.0f, 0.0f));
  // player translation
  player_mat = glm::translate(player_mat, glm::vec3(player_state_->pos().x, player_state_->pos().y, 0.0f));
  // player rotation
  player_mat = glm::rotate(player_mat, player_state_->orientation(), glm::vec3(0.0f, 0.0f, 1.0f));

  draw_player(player_mat);
}

void MapRenderer::draw_map(const glm::mat4 &mat) const {
  for (auto v_it = std::size_t{0u}; v_it < vector_map_->vectors().size(); v_it++) {
    const auto &c = vector_map_->colors()[v_it];
    SDL_SetRenderDrawColor(r(), c.r, c.g, c.b, 255);

    const auto &v = vector_map_->vectors()[v_it];
    const auto beg = mat * glm::vec4(v.first, 0.0f, 1.0f);
    const auto end = mat * glm::vec4(v.second, 0.0f, 1.0f);
    SDL_RenderDrawLineF(r(), beg.x, beg.y, end.x, end.y);
  }
}

void MapRenderer::draw_player(const glm::mat4 &mat) const {
  constexpr auto ray_length = 5.0f;
  const auto zero = mat * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
  const auto left = mat * glm::vec4(std::cosf(-player_state_->fov_rad() / 2.0f) * ray_length,
                                    std::sinf(-player_state_->fov_rad() / 2.0f) * ray_length,
                                    0.0f,
                                    1.0f);
  const auto right = mat * glm::vec4(std::cosf(player_state_->fov_rad() / 2.0f) * ray_length,
                                     std::sinf(player_state_->fov_rad() / 2.0f) * ray_length,
                                     0.0f,
                                     1.0f);
  const auto front = mat * glm::vec4(ray_length, 0.0f, 0.0f, 1.0f);
  SDL_SetRenderDrawColor(r(), 0, 0, 0, 255);
  SDL_RenderDrawLineF(r(), zero.x, zero.y, left.x, left.y);
  SDL_RenderDrawLineF(r(), zero.x, zero.y, right.x, right.y);
  SDL_SetRenderDrawColor(r(), 128, 0, 0, 255);
  SDL_RenderDrawLineF(r(), zero.x, zero.y, front.x, front.y);
}
} // namespace wolf
