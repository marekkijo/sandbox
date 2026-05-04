#include "raycaster_scene.hpp"

#include "wolf_common/map_utils.hpp"

#include <algorithm>
#include <cmath>

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
  constexpr auto orientation_shadow = 0.625f;
  constexpr auto num_steps = 8;
  constexpr auto max_proximity_shadow = 0.75f;
  constexpr auto step_size = max_proximity_shadow / num_steps;

  const auto &rays = raycaster_.rays();
  const auto strip_width = static_cast<float>(width_) / static_cast<float>(rays.size());
  for (std::size_t ray_index = 0; ray_index < rays.size(); ++ray_index) {
    const auto &ray = rays[ray_index];
    if (ray.wall == Map::Walls::nothing) {
      continue;
    }

    const auto height_multiplier = ray.dist > 0.0f ? 1.0f / ray.dist : 1.0f;
    const auto projected_height = height_multiplier * static_cast<float>(height_);
    const auto wall_strip = SDL_FRect{ray_index * strip_width,
                                      (static_cast<float>(height_) - projected_height) / 2.0f,
                                      strip_width,
                                      projected_height};

    const auto base_color = MapUtils::wall_color(ray.wall);

    // E/W faces (x-facing) are rendered darker, matching the top-down map shading.
    const auto orient_factor = ray.x_facing ? orientation_shadow : 1.0f;

    // Proximity shading: walls darken as you approach them.
    // Quantize the shadow amount so that the full [0.25, 1.0] brightness range is reachable.
    const auto raw_shadow = std::min(max_proximity_shadow, height_multiplier);
    const auto quantized_shadow = static_cast<float>(static_cast<int>(raw_shadow / step_size)) * step_size;
    const auto proximity_factor = 1.0f - quantized_shadow;

    const auto combined = orient_factor * proximity_factor;
    const auto color = glm::uvec3{static_cast<std::uint32_t>(std::round(static_cast<float>(base_color.r) * combined)),
                                  static_cast<std::uint32_t>(std::round(static_cast<float>(base_color.g) * combined)),
                                  static_cast<std::uint32_t>(std::round(static_cast<float>(base_color.b) * combined))};
    r().set_color(color);
    r().fill_rect(wall_strip);
  }
}
} // namespace wolf
