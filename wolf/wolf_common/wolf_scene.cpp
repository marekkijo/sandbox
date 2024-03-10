#include "wolf_scene.hpp"

#include <glm/glm.hpp>

#include <cmath>

namespace wolf {

WolfScene::WolfScene(const RawMap &raw_map, const std::uint32_t fov_in_degrees, const std::uint32_t number_of_rays)
    : vector_map_{std::make_shared<VectorMap>(raw_map)}
    , player_state_{std::make_shared<PlayerState>(raw_map)}
    , map_renderer_{std::make_unique<MapRenderer>(vector_map_, player_state_, fov_in_degrees)}
    , last_timestamp_ms_{timestamp()}
    , walls_(number_of_rays)
    , ray_rots_(number_of_rays) {
  const auto fov_in_rad = glm::radians(static_cast<float>(fov_in_degrees));

  const auto fov_step = fov_in_rad / (ray_rots_.size() - 1u);
  for (auto r_it = std::size_t{0u}; r_it < ray_rots_.size(); r_it++) {
    const auto angle = -(fov_in_rad / 2.0f) + fov_step * r_it;
    ray_rots_[r_it].sin = std::sinf(angle);
    ray_rots_[r_it].cos = std::cosf(angle);
  }
}

void WolfScene::loop(const gp::misc::Event &event) {
  switch (event.type()) {
  case gp::misc::Event::Type::Init:
    initialize(event.init().width, event.init().height);
    break;
  case gp::misc::Event::Type::Quit:
    finalize();
    break;
  case gp::misc::Event::Type::Resize:
    resize(event.resize().width, event.resize().height);
    break;
  case gp::misc::Event::Type::Redraw: {
    const auto time_elapsed_ms = event.timestamp() - last_timestamp_ms_;
    animate(time_elapsed_ms);
    last_timestamp_ms_ = event.timestamp();
    redraw();
  } break;
  default:
    break;
  }
}

void WolfScene::initialize(const int width, const int height) {
  player_state_->set_keyboard_state(keyboard_state());
  map_renderer_->set_renderer(renderer());
  resize(width, height);
}

void WolfScene::finalize() {
  map_renderer_->set_renderer(nullptr);
  player_state_->set_keyboard_state(nullptr);
}

void WolfScene::resize(const int width, const int height) {
  width_ = width;
  height_ = height;

  const auto w_part = width_ / static_cast<double>(walls_.size());
  const auto w = static_cast<int>(std::ceil(w_part));
  for (auto w_it = std::size_t{0u}; w_it < walls_.size(); w_it++) {
    walls_[w_it].rect.x = static_cast<int>(std::floor(w_part * w_it));
    walls_[w_it].rect.w = w;
  }

  map_renderer_->resize(width, height);
}

void WolfScene::animate(const std::uint32_t time_elapsed_ms) { player_state_->animate(time_elapsed_ms); }

void WolfScene::redraw() {
  r().set_draw_color(0, 0, 0, 255);
  r().clear();

  prepare_walls();
  draw_background();
  draw_walls();

  map_renderer_->redraw();

  r().present();
}

void WolfScene::draw_background() const {
  const auto ceiling_rect = SDL_Rect{0, 0, width_, height_ / 2};
  const auto floor_rect = SDL_Rect{0, height_ / 2, width_, height_ / 2};

  r().set_draw_color(57, 57, 57, 255);
  r().fill_rect(ceiling_rect);

  r().set_draw_color(115, 115, 115, 255);
  r().fill_rect(floor_rect);
}

void WolfScene::draw_walls() const {
  auto rects = std::vector<SDL_Rect>{};
  rects.reserve(walls_.size());

  auto last_color = vector_map_->color(walls_[0].color_index, walls_[0].shadow_factor);
  r().set_draw_color(last_color.r, last_color.g, last_color.b, 255);

  for (const auto &wall : walls_) {
    const auto color = vector_map_->color(wall.color_index, wall.shadow_factor);
    if (color == last_color) {
      rects.push_back(wall.rect);
      continue;
    }

    r().fill_rects(rects.data(), rects.size());
    rects.clear();

    last_color = color;
    r().set_draw_color(color.r, color.g, color.b, 255);
    rects.push_back(wall.rect);
  }

  r().fill_rects(rects.data(), rects.size());
  rects.clear();
}
} // namespace wolf
