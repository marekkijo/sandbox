#include "raycaster_scene.hpp"

#include "wolf_common/map_utils.hpp"

#include <algorithm>
#include <cmath>

namespace wolf {
RaycasterScene::RaycasterScene(std::unique_ptr<const RawMap> raw_map,
                               std::shared_ptr<const VswapFile> vswap_file,
                               const int fov_in_degrees,
                               const int num_rays)
    : raw_map_{std::move(raw_map)}
    , vswap_file_{std::move(vswap_file)}
    , raycaster_{*raw_map_, player_state_, fov_in_degrees, num_rays} {}

void RaycasterScene::loop(const gp::misc::Event &event) {
  switch (event.type()) {
  case gp::misc::Event::Type::Init:
    last_timestamp_ms_ = event.timestamp();
    player_state_.set_keyboard_state(keyboard_state());
    sdl_r_ = renderer()->sdl_renderer();
    init_wall_textures();
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

void RaycasterScene::init_wall_textures() {
  const auto &walls = vswap_file_->walls();
  wall_textures_.reserve(walls.size());
  for (const auto &tex_data : walls) {
    auto *tex = SDL_CreateTexture(sdl_r_, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, 64, 64);
    SDL_UpdateTexture(tex, nullptr, tex_data.data(), 64 * 4);
    SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_NONE);
    wall_textures_.emplace_back(tex, SDL_DestroyTexture);
  }
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
    const auto wall_top = (static_cast<float>(height_) - projected_height) / 2.0f;
    const auto wall_strip = SDL_FRect{ray_index * strip_width, wall_top, strip_width, projected_height};

    // Select texture: two variants per wall type; even = N/S (light), odd = E/W (dark).
    const auto wall_val = static_cast<std::size_t>(ray.wall);
    const auto tex_idx = (wall_val - 1) * 2 + (ray.x_facing ? 1 : 0);

    if (tex_idx >= wall_textures_.size()) {
      continue;
    }

    // Source column from the 64×64 texture based on the hit u-coordinate.
    const auto tex_col = static_cast<int>(ray.tex_u * 64.0f);
    const auto src = SDL_FRect{static_cast<float>(tex_col), 0.0f, 1.0f, 64.0f};

    // Proximity shading via texture colour mod (discretised to reduce banding).
    const auto raw_proximity = 1.0f - std::min(max_proximity_shadow, height_multiplier);
    const auto proximity_factor = static_cast<float>(static_cast<int>(raw_proximity / step_size)) * step_size;
    const auto shade = static_cast<std::uint8_t>(std::round(proximity_factor * 255.0f));

    auto *tex = wall_textures_[tex_idx].get();
    SDL_SetTextureColorMod(tex, shade, shade, shade);
    SDL_RenderTexture(sdl_r_, tex, &src, &wall_strip);
  }
}
} // namespace wolf
