#include "raycaster_scene.hpp"

#include "wolf_common/map_utils.hpp"

#include <algorithm>
#include <cmath>
#include <format>
#include <stdexcept>

#include <glm/vec3.hpp>

namespace wolf {
constexpr auto max_level = 6;

RaycasterScene::RaycasterScene(std::unique_ptr<const RawMap> raw_map,
                               std::shared_ptr<const VswapFile> vswap_file,
                               const int fov_in_degrees)
    : raw_map_{std::move(raw_map)}
    , vswap_file_{std::move(vswap_file)}
    , raycaster_{*raw_map_, player_state_, fov_in_degrees, 1}
    , map_renderer_{vector_map_, player_state_, static_cast<std::uint32_t>(fov_in_degrees)} {}

void RaycasterScene::loop(const gp::misc::Event &event) {
  switch (event.type()) {
  case gp::misc::Event::Type::Init:
    last_timestamp_ms_ = event.timestamp();
    player_state_.set_keyboard_state(keyboard_state());
    sdl_r_ = renderer()->sdl_renderer();
    map_renderer_.set_renderer(renderer());
    init_wall_textures();
    resize(event.init().width, event.init().height);
    break;
  case gp::misc::Event::Type::Quit:
    map_renderer_.set_renderer(nullptr);
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
  case gp::misc::Event::Type::Key:
    if (event.key().action == gp::misc::Event::Action::Pressed) {
      switch (event.key().scan_code) {
      case gp::misc::Event::ScanCode::T:
        show_textures_ = !show_textures_;
        break;
      case gp::misc::Event::ScanCode::G:
        show_proximity_shading_ = !show_proximity_shading_;
        break;
      case gp::misc::Event::ScanCode::L:
        show_orientation_shading_ = !show_orientation_shading_;
        break;
      case gp::misc::Event::ScanCode::M:
        show_map_ = !show_map_;
        break;
      case gp::misc::Event::ScanCode::O:
        map_renderer_.set_player_oriented(!map_player_oriented_);
        map_player_oriented_ = !map_player_oriented_;
        break;
      case gp::misc::Event::ScanCode::LeftBracket:
        rays_level_ = std::min(max_level, rays_level_ + 1);
        raycaster_.set_num_rays(std::max(1, width_ >> rays_level_));
        break;
      case gp::misc::Event::ScanCode::RightBracket:
        rays_level_ = std::max(0, rays_level_ - 1);
        raycaster_.set_num_rays(std::max(1, width_ >> rays_level_));
        break;
      case gp::misc::Event::ScanCode::Minus:
        h_lines_level_ = std::min(max_level, h_lines_level_ + 1);
        break;
      case gp::misc::Event::ScanCode::Equals:
        h_lines_level_ = std::max(0, h_lines_level_ - 1);
        break;
      default:
        break;
      }
    }
    break;
  default:
    break;
  }
}

void RaycasterScene::resize(const int width, const int height) {
  width_ = width;
  height_ = height;
  map_renderer_.resize(width, height);
  raycaster_.set_num_rays(std::max(1, width_ >> rays_level_));
}

void RaycasterScene::init_wall_textures() {
  if (!vswap_file_) {
    throw std::runtime_error("VswapFile is null: cannot initialize wall textures");
  }
  const auto &walls = vswap_file_->walls();
  wall_textures_.reserve(walls.size());
  for (const auto &tex_data : walls) {
    auto *tex = SDL_CreateTexture(sdl_r_, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, 64, 64);
    if (!tex) {
      throw std::runtime_error(std::string("SDL_CreateTexture failed: ") + SDL_GetError());
    }
    if (!SDL_UpdateTexture(tex, nullptr, tex_data.data(), 64 * 4)) {
      SDL_DestroyTexture(tex);
      throw std::runtime_error(std::string("SDL_UpdateTexture failed: ") + SDL_GetError());
    }
    SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_NONE);
    SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);
    wall_textures_.emplace_back(tex, SDL_DestroyTexture);
  }
}

void RaycasterScene::redraw() {
  r().set_color(0, 0, 0);
  r().clear();

  draw_background();
  draw_walls();

  if (show_map_) {
    map_renderer_.redraw();
  }

  draw_help_overlay();

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
  const auto quantize = h_lines_level_ > 0;
  const auto num_h_lines = height_ >> h_lines_level_;
  const auto cell_h = quantize ? static_cast<float>(height_) / static_cast<float>(num_h_lines) : 1.0f;

  for (std::size_t ray_index = 0; ray_index < rays.size(); ++ray_index) {
    const auto &ray = rays[ray_index];
    if (ray.wall == Map::Walls::nothing) {
      continue;
    }

    const auto height_multiplier = ray.dist > 0.0f ? 1.0f / ray.dist : 1.0f;
    const auto projected_height = height_multiplier * static_cast<float>(height_);
    const auto wall_top = (static_cast<float>(height_) - projected_height) / 2.0f;

    auto actual_top = wall_top;
    auto actual_height = projected_height;
    if (quantize) {
      actual_top = std::floor(wall_top / cell_h) * cell_h;
      actual_height = std::ceil((wall_top + projected_height) / cell_h) * cell_h - actual_top;
    }

    const auto wall_strip = SDL_FRect{ray_index * strip_width, actual_top, strip_width, actual_height};

    if (show_textures_) {
      // Orientation: pick E/W (dark) variant when enabled, otherwise always use N/S (light).
      const auto wall_val = static_cast<std::size_t>(ray.wall);
      const auto tex_face = show_orientation_shading_ ? (ray.x_facing ? 1u : 0u) : 0u;
      const auto tex_idx = (wall_val - 1) * 2 + tex_face;

      const auto proximity_shade = [&]() -> std::uint8_t {
        if (!show_proximity_shading_) {
          return 255u;
        }
        const auto raw = 1.0f - std::min(max_proximity_shadow, height_multiplier);
        const auto stepped = static_cast<float>(static_cast<int>(raw / step_size)) * step_size;
        return static_cast<std::uint8_t>(std::round(stepped * 255.0f));
      }();

      if (tex_idx < wall_textures_.size()) {
        // Source column from the 64×64 texture based on the hit u-coordinate.
        const auto tex_col = static_cast<float>(std::clamp(static_cast<int>(ray.tex_u * 64.0f), 0, 63));

        auto *tex = wall_textures_[tex_idx].get();
        SDL_SetTextureColorMod(tex, proximity_shade, proximity_shade, proximity_shade);

        if (quantize) {
          // Render one texel per virtual cell so every cell shows a single solid colour.
          const auto first_cell = static_cast<int>(std::round(actual_top / cell_h));
          const auto last_cell = static_cast<int>(std::round((actual_top + actual_height) / cell_h)) - 1;
          for (auto cell = first_cell; cell <= last_cell; ++cell) {
            const auto cell_y = static_cast<float>(cell) * cell_h;
            const auto t = std::clamp((cell_y + 0.5f * cell_h - wall_top) / projected_height, 0.0f, 1.0f);
            // Floor to an integer texel row so the 1×1 source rect covers exactly one texel
            // and SDL's upscale maps every destination row to the same single colour.
            const auto tex_y = std::floor(t * 64.0f);
            const auto src = SDL_FRect{tex_col, tex_y, 1.0f, 1.0f};
            const auto dst = SDL_FRect{ray_index * strip_width, cell_y, strip_width, cell_h};
            SDL_RenderTexture(sdl_r_, tex, &src, &dst);
          }
        } else {
          const auto src = SDL_FRect{tex_col, 0.0f, 1.0f, 64.0f};
          SDL_RenderTexture(sdl_r_, tex, &src, &wall_strip);
        }
      } else {
        // Fallback for wall types without a VSWAP texture pair (e.g. doors, elevator).
        r().set_color(proximity_shade, proximity_shade, proximity_shade);
        r().fill_rect(wall_strip);
      }
    } else {
      // Solid color: orientation and proximity shading are independent multipliers.
      const auto base_color = MapUtils::wall_color(ray.wall);
      auto shadow = show_orientation_shading_ ? (ray.x_facing ? MapUtils::orientation_shadow_factor : 1.0f) : 1.0f;
      if (show_proximity_shading_) {
        const auto raw = 1.0f - std::min(max_proximity_shadow, height_multiplier);
        const auto stepped = static_cast<float>(static_cast<int>(raw / step_size)) * step_size;
        shadow *= stepped;
      }
      r().set_color(glm::uvec3{static_cast<unsigned>(std::round(base_color.r * shadow)),
                               static_cast<unsigned>(std::round(base_color.g * shadow)),
                               static_cast<unsigned>(std::round(base_color.b * shadow))});
      r().fill_rect(wall_strip);
    }
  }
}

void RaycasterScene::draw_help_overlay() const {
  constexpr auto scale = 2.0f;
  constexpr auto ch = 8; // SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE
  constexpr auto line_h = ch + 4;
  constexpr auto padding = 6;
  constexpr auto num_lines = 7;
  constexpr auto max_chars = 22;

  float prev_sx{}, prev_sy{};
  SDL_GetRenderScale(sdl_r_, &prev_sx, &prev_sy);
  SDL_SetRenderScale(sdl_r_, scale, scale);

  const auto log_h = static_cast<float>(height_) / scale;
  const auto bg_w = static_cast<float>(max_chars * ch + padding * 2);
  const auto bg_h = static_cast<float>(num_lines * line_h - 4 + padding * 2);
  const auto bg_x = static_cast<float>(padding);
  const auto bg_y = log_h - bg_h - static_cast<float>(padding);

  SDL_SetRenderDrawBlendMode(sdl_r_, SDL_BLENDMODE_BLEND);
  SDL_SetRenderDrawColor(sdl_r_, 0, 0, 0, 180);
  const SDL_FRect bg{bg_x, bg_y, bg_w, bg_h};
  SDL_RenderFillRect(sdl_r_, &bg);
  SDL_SetRenderDrawBlendMode(sdl_r_, SDL_BLENDMODE_NONE);

  SDL_SetRenderDrawColor(sdl_r_, 220, 220, 220, 255);
  const auto tx = bg_x + padding;
  auto ty = bg_y + padding;
  SDL_RenderDebugText(sdl_r_, tx, ty, show_textures_ ? "T: Textures      [on ]" : "T: Textures      [off]");
  ty += line_h;
  SDL_RenderDebugText(sdl_r_, tx, ty, show_proximity_shading_ ? "G: Prox shading  [on ]" : "G: Prox shading  [off]");
  ty += line_h;
  SDL_RenderDebugText(sdl_r_, tx, ty, show_orientation_shading_ ? "L: Orient shade  [on ]" : "L: Orient shade  [off]");
  ty += line_h;
  SDL_RenderDebugText(sdl_r_, tx, ty, show_map_ ? "M: Map           [on ]" : "M: Map           [off]");
  ty += line_h;
  SDL_RenderDebugText(sdl_r_, tx, ty, map_player_oriented_ ? "O: Map    [player-up]" : "O: Map     [north-up]");
  ty += line_h;
  const auto level_label = [](const int level) -> std::string {
    if (level == 0) {
      return "full";
    }
    return std::format("{:>4}", std::format("1/{}", 1 << level));
  };
  SDL_RenderDebugText(sdl_r_, tx, ty, std::format("[/]: Rays       [{}]", level_label(rays_level_)).c_str());
  ty += line_h;
  SDL_RenderDebugText(sdl_r_, tx, ty, std::format("-/=: H-lines    [{}]", level_label(h_lines_level_)).c_str());

  SDL_SetRenderScale(sdl_r_, prev_sx, prev_sy);
}

} // namespace wolf
