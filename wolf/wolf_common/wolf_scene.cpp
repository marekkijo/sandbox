#include "wolf_scene.hpp"

#include <glm/glm.hpp>

#include <cmath>

namespace wolf {
WolfScene::WolfScene(std::unique_ptr<const RawMap> raw_map,
                     const std::uint32_t fov_in_degrees,
                     const std::uint32_t number_of_rays)
    : raw_map_{std::move(raw_map)}
    , map_renderer_{vector_map_, player_state_, fov_in_degrees}
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
  player_state_.set_keyboard_state(keyboard_state());
  map_renderer_.set_renderer(renderer());
  resize(width, height);
}

void WolfScene::finalize() {
  map_renderer_.set_renderer(nullptr);
  player_state_.set_keyboard_state(nullptr);
}

void WolfScene::resize(const int width, const int height) {
  width_ = static_cast<float>(width);
  height_ = static_cast<float>(height);

  const auto w_part = width_ / walls_.size();
  const auto w = std::ceil(w_part);
  for (auto w_it = 0; w_it < walls_.size(); ++w_it) {
    walls_[w_it].rect.x = std::floor(w_part * w_it);
    walls_[w_it].rect.w = w;
  }

  map_renderer_.resize(width, height);
}

void WolfScene::animate(const std::uint64_t time_elapsed_ms) { player_state_.animate(time_elapsed_ms); }

void WolfScene::redraw() {
  r().set_color(0, 0, 0);
  r().clear();

  prepare_walls();
  draw_background();
  draw_walls();

  map_renderer_.redraw();

  r().present();
}

void WolfScene::draw_background() const {
  const auto ceiling_rect = SDL_FRect{0.0f, 0.0f, width_, height_ / 2.0f};
  const auto floor_rect = SDL_FRect{0.0f, height_ / 2.0f, width_, height_ / 2.0f};

  r().set_color(57, 57, 57);
  r().fill_rect(ceiling_rect);

  r().set_color(115, 115, 115);
  r().fill_rect(floor_rect);
}

void WolfScene::draw_walls() const {
  auto rects = std::vector<SDL_FRect>{};
  rects.reserve(walls_.size());

  auto last_color = vector_map_.color(walls_[0].color_index, walls_[0].shadow_factor);
  r().set_color(last_color);

  for (const auto &wall : walls_) {
    const auto color = vector_map_.color(wall.color_index, wall.shadow_factor);
    if (color == last_color) {
      rects.push_back(wall.rect);
      continue;
    }

    r().fill_rects(rects.data(), static_cast<int>(rects.size()));
    rects.clear();

    last_color = color;
    r().set_color(last_color);
    rects.push_back(wall.rect);
  }

  r().fill_rects(rects.data(), static_cast<int>(rects.size()));
  rects.clear();
}
} // namespace wolf
