#include "map_renderer.hpp"

#include <glm/ext/matrix_transform.hpp>

#include <numbers>

namespace wolf {
MapRenderer::MapRenderer(tools::sdl::SDLSystem             &sdl_sys,
                         std::shared_ptr<const VectorMap>   vector_map,
                         std::shared_ptr<const PlayerState> player_state)
    : Renderer(sdl_sys), vector_map_{vector_map}, player_state_{player_state} {
  Renderer::resize();
}

void MapRenderer::redraw() {
  if (player_oriented()) {
    redraw_player_oriented();
  } else {
    redraw_map_oriented();
  }
}

void MapRenderer::resize(int width, int height) {
  translate_x_ = static_cast<float>(width) / 2.0f;
  translate_y_ = static_cast<float>(height) / 2.0f;
  scale_       = std::min(width, height) / vector_map().diagonal_length();
}

const VectorMap &MapRenderer::vector_map() const { return *vector_map_; }

const PlayerState &MapRenderer::player_state() const { return *player_state_; }

bool MapRenderer::player_oriented() const { return player_oriented_; }

float MapRenderer::translate_x() const { return translate_x_; }

float MapRenderer::translate_y() const { return translate_y_; }

std::tuple<float, float> MapRenderer::translate() const { return std::make_tuple(translate_x(), translate_y()); }

float MapRenderer::scale() const { return scale_; }

void MapRenderer::redraw_player_oriented() const {
  // to the center of the screen
  auto map_mat = glm::translate(glm::mat4(1.0f), glm::vec3(translate_x(), translate_y(), 0.0f));
  // orient to north
  map_mat      = glm::rotate(map_mat, -static_cast<float>(std::numbers::pi / 2.0f), glm::vec3(0.0f, 0.0f, 1.0f));
  // scale map
  map_mat      = glm::scale(map_mat, glm::vec3(scale()));
  // player rotation
  map_mat      = glm::rotate(map_mat, -player_state().orientation(), glm::vec3(0.0f, 0.0f, 1.0f));
  // player translation
  map_mat      = glm::translate(map_mat, glm::vec3(-player_state().pos_x(), -player_state().pos_y(), 0.0f));

  draw_map(map_mat);

  // to the center of the screen
  auto player_mat = glm::translate(glm::mat4(1.0f), glm::vec3(translate_x(), translate_y(), 0.0f));
  // orient to north
  player_mat      = glm::rotate(player_mat, -static_cast<float>(std::numbers::pi / 2.0f), glm::vec3(0.0f, 0.0f, 1.0f));
  // scale map
  player_mat      = glm::scale(player_mat, glm::vec3(scale()));
  draw_player(player_mat);
}

void MapRenderer::redraw_map_oriented() const {
  // to the center of the screen
  auto map_mat = glm::translate(glm::mat4(1.0f), glm::vec3(translate_x(), translate_y(), 0.0f));
  // scale map
  map_mat      = glm::scale(map_mat, glm::vec3(scale()));
  // to the center of the map
  map_mat      = glm::translate(map_mat, glm::vec3(-vector_map().width() / 2.0f, -vector_map().height() / 2.0f, 0.0f));
  draw_map(map_mat);

  // to the center of the screen
  auto player_mat = glm::translate(glm::mat4(1.0f), glm::vec3(translate_x(), translate_y(), 0.0f));
  // scale map
  player_mat      = glm::scale(player_mat, glm::vec3(scale()));
  // to the center of the map
  player_mat = glm::translate(player_mat, glm::vec3(-vector_map().width() / 2.0f, -vector_map().height() / 2.0f, 0.0f));
  // player translation
  player_mat = glm::translate(player_mat, glm::vec3(player_state().pos_x(), player_state().pos_y(), 0.0f));
  // player rotation
  player_mat = glm::rotate(player_mat, player_state().orientation(), glm::vec3(0.0f, 0.0f, 1.0f));
  draw_player(player_mat);
}

void MapRenderer::draw_map(const glm::mat4 &mat) const {
  for (const auto &v : vector_map().vectors()) {
    const auto beg = mat * glm::vec4(v.first, 0.0f, 1.0f);
    const auto end = mat * glm::vec4(v.second, 0.0f, 1.0f);
    SDL_SetRenderDrawColor(r(), 255, 0, 0, 255);
    SDL_RenderDrawLineF(r(), beg.x, beg.y, end.x, end.y);
  }
}

void MapRenderer::draw_player(const glm::mat4 &mat) const {
  constexpr auto ray_length = 5.0f;
  const auto     zero       = mat * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
  const auto     left       = mat * glm::vec4(std::cosf(-player_state().fov_rad() / 2.0f) * ray_length,
                                    std::sinf(-player_state().fov_rad() / 2.0f) * ray_length,
                                    0.0f,
                                    1.0f);
  const auto     right      = mat * glm::vec4(std::cosf(player_state().fov_rad() / 2.0f) * ray_length,
                                     std::sinf(player_state().fov_rad() / 2.0f) * ray_length,
                                     0.0f,
                                     1.0f);
  const auto     front      = mat * glm::vec4(ray_length, 0.0f, 0.0f, 1.0f);
  SDL_SetRenderDrawColor(r(), 0, 0, 0, 255);
  SDL_RenderDrawLineF(r(), zero.x, zero.y, left.x, left.y);
  SDL_RenderDrawLineF(r(), zero.x, zero.y, right.x, right.y);
  SDL_SetRenderDrawColor(r(), 128, 0, 0, 255);
  SDL_RenderDrawLineF(r(), zero.x, zero.y, front.x, front.y);
}
} // namespace wolf
