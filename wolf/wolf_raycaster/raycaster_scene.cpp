#include "raycaster_scene.hpp"

namespace wolf {
RaycasterScene::RaycasterScene(std::unique_ptr<const RawMap> raw_map, const int fov_in_degrees, const int num_rays)
    : raw_map_{std::move(raw_map)}
    , raycaster_{*raw_map_, player_state_, fov_in_degrees, num_rays} {}

void RaycasterScene::loop(const gp::misc::Event &event) {
  switch (event.type()) {
  case gp::misc::Event::Type::Init:
    last_timestamp_ms_ = event.timestamp();
    player_state_.set_keyboard_state(keyboard_state());
    resize(event.init().width, event.init().height);
    break;
  case gp::misc::Event::Type::Quit:
    player_state_.set_keyboard_state(nullptr);
    break;
  case gp::misc::Event::Type::Resize:
    resize(event.resize().width, event.resize().height);
    break;
  case gp::misc::Event::Type::Redraw: {
    const auto time_elapsed_ms = event.timestamp() - last_timestamp_ms_;
    player_state_.animate(time_elapsed_ms);
    raycaster_.cast_rays();
    last_timestamp_ms_ = event.timestamp();
    redraw();
  } break;
  default:
    break;
  }
}

void RaycasterScene::resize(const int width, const int height) {
  width_ = width;
  height_ = height;
}

void RaycasterScene::redraw() {
  r().set_color(0, 0, 0);
  r().clear();

  draw_background();
  draw_walls();

  r().present();
}

void RaycasterScene::draw_background() const {
  const auto ceiling_rect = SDL_FRect{0.0f, 0.0f, static_cast<float>(width_), height_ / 2.0f};
  const auto floor_rect = SDL_FRect{0.0f, height_ / 2.0f, static_cast<float>(width_), height_ / 2.0f};

  r().set_color(57, 57, 57);
  r().fill_rect(ceiling_rect);

  r().set_color(115, 115, 115);
  r().fill_rect(floor_rect);
}

void RaycasterScene::draw_walls() const {
  const auto &rays = raycaster_.rays();
  const auto strip_width = static_cast<float>(width_) / static_cast<float>(rays.size());
  for (std::size_t ray_index = 0; ray_index < rays.size(); ++ray_index) {
    const auto &ray = rays[ray_index];
    const auto height_multiplier = ray.dist > 0.0f ? 1.0f / ray.dist : 1.0f;
    const auto projected_height = height_multiplier * static_cast<float>(height_);
    const auto wall_strip = SDL_FRect{ray_index * strip_width,
                                      (static_cast<float>(height_) - projected_height) / 2.0f,
                                      strip_width,
                                      projected_height};
    r().set_color(192, 192, 192);
    r().fill_rect(wall_strip);
  }
}
} // namespace wolf
