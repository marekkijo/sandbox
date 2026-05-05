#include "raycaster_scene.hpp"

#include "wolf_common/map_utils.hpp"

#include <algorithm>
#include <cmath>
#include <format>
#include <stdexcept>

#include <glm/vec3.hpp>

namespace {
struct LevelInfo {
  int num;
  int den;
  const char *label;
};

constexpr std::array k_levels = {
    LevelInfo{1,  1, "full"},
    LevelInfo{3,  4, " 3/4"},
    LevelInfo{1,  2, " 1/2"},
    LevelInfo{3,  8, " 3/8"},
    LevelInfo{1,  4, " 1/4"},
    LevelInfo{3, 16, "3/16"},
    LevelInfo{1,  8, " 1/8"},
    LevelInfo{1, 16, "1/16"},
    LevelInfo{1, 32, "1/32"},
    LevelInfo{1, 64, "1/64"},
};
constexpr auto max_level = static_cast<int>(k_levels.size()) - 1;

int level_count(const int dim, const int level) { return std::max(1, dim * k_levels[level].num / k_levels[level].den); }
} // anonymous namespace

namespace wolf {

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
        rebuild_vscreen();
        raycaster_.set_num_rays(level_count(width_, rays_level_));
        break;
      case gp::misc::Event::ScanCode::RightBracket:
        rays_level_ = std::max(0, rays_level_ - 1);
        rebuild_vscreen();
        raycaster_.set_num_rays(level_count(width_, rays_level_));
        break;
      case gp::misc::Event::ScanCode::Minus:
        h_lines_level_ = std::min(max_level, h_lines_level_ + 1);
        rebuild_vscreen();
        break;
      case gp::misc::Event::ScanCode::Equals:
        h_lines_level_ = std::max(0, h_lines_level_ - 1);
        rebuild_vscreen();
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
  raycaster_.set_num_rays(level_count(width_, rays_level_));
  rebuild_vscreen();
}

void RaycasterScene::rebuild_vscreen() {
  const auto vw = level_count(width_, rays_level_);
  const auto vh = level_count(height_, h_lines_level_);
  pixel_buf_.assign(static_cast<std::size_t>(vw * vh), 0u);
  auto *tex = SDL_CreateTexture(sdl_r_, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, vw, vh);
  if (!tex) {
    throw std::runtime_error(std::string("SDL_CreateTexture failed: ") + SDL_GetError());
  }
  SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);
  vscreen_tex_.reset(tex);
}

void RaycasterScene::redraw() {
  r().set_color(0, 0, 0);
  r().clear();
  draw_virtual_screen();
  if (show_map_) {
    map_renderer_.redraw();
  }
  draw_help_overlay();
  r().present();
}

void RaycasterScene::draw_virtual_screen() {
  constexpr auto num_steps = 8;
  constexpr auto max_proximity_shadow = 0.75f;
  constexpr auto step_size = max_proximity_shadow / num_steps;

  const auto vw = level_count(width_, rays_level_);
  const auto vh = level_count(height_, h_lines_level_);
  const auto cell_h_f = float(height_) / float(vh);

  // Background fill
  constexpr std::uint32_t ceil_px = 57u | (57u << 8) | (57u << 16) | (0xFFu << 24);
  constexpr std::uint32_t floor_px = 115u | (115u << 8) | (115u << 16) | (0xFFu << 24);
  for (int vy = 0; vy < vh; ++vy) {
    std::fill_n(pixel_buf_.data() + vy * vw, vw, vy < vh / 2 ? ceil_px : floor_px);
  }

  // Wall pixels
  const auto &rays = raycaster_.rays();
  const auto &wall_data = vswap_file_->walls();

  for (int rx = 0; rx < vw; ++rx) {
    if (rx >= static_cast<int>(rays.size())) {
      break;
    }
    const auto &ray = rays[static_cast<std::size_t>(rx)];
    if (ray.wall == Map::Walls::nothing) {
      continue;
    }

    const auto height_mult = ray.dist > 0.0f ? 1.0f / ray.dist : 1.0f;
    const auto proj_h = height_mult * float(height_);
    const auto wall_top = (float(height_) - proj_h) / 2.0f;
    const auto wall_bot = wall_top + proj_h;

    const auto vy_min = std::max(0, static_cast<int>(wall_top / cell_h_f));
    const auto vy_max = std::min(vh - 1, static_cast<int>(std::ceil(wall_bot / cell_h_f)));

    std::uint32_t proximity_shade = 255u;
    if (show_proximity_shading_) {
      const auto raw = 1.0f - std::min(max_proximity_shadow, height_mult);
      const auto stepped = float(int(raw / step_size)) * step_size;
      proximity_shade = std::uint32_t(std::round(stepped * 255.0f));
    }

    if (show_textures_) {
      const auto wall_val = static_cast<std::size_t>(ray.wall);
      const auto tex_face = show_orientation_shading_ ? (ray.x_facing ? 1u : 0u) : 0u;
      const auto tex_idx = (wall_val - 1u) * 2u + tex_face;
      const auto tex_col = std::clamp(static_cast<int>(ray.tex_u * 64.0f), 0, 63);

      if (tex_idx < wall_data.size()) {
        for (int vy = vy_min; vy <= vy_max; ++vy) {
          const auto screen_y = (float(vy) + 0.5f) * cell_h_f;
          if (screen_y < wall_top || screen_y >= wall_bot) {
            continue;
          }
          const auto t = (screen_y - wall_top) / proj_h;
          const auto tex_row = std::clamp(static_cast<int>(t * 64.0f), 0, 63);
          const auto &texel = wall_data[tex_idx][static_cast<std::size_t>(tex_row * 64 + tex_col)];
          auto px = std::uint32_t(texel.r) | (std::uint32_t(texel.g) << 8u) | (std::uint32_t(texel.b) << 16u) |
                    (0xFFu << 24u);
          if (proximity_shade < 255u) {
            const auto r = (px & 0xFFu) * proximity_shade / 255u;
            const auto g = ((px >> 8u) & 0xFFu) * proximity_shade / 255u;
            const auto b = ((px >> 16u) & 0xFFu) * proximity_shade / 255u;
            px = (0xFFu << 24u) | (b << 16u) | (g << 8u) | r;
          }
          pixel_buf_[static_cast<std::size_t>(vy * vw + rx)] = px;
        }
      } else {
        // Fallback: grey based on proximity_shade
        const auto grey_px = proximity_shade | (proximity_shade << 8u) | (proximity_shade << 16u) | (0xFFu << 24u);
        for (int vy = vy_min; vy <= vy_max; ++vy) {
          const auto screen_y = (float(vy) + 0.5f) * cell_h_f;
          if (screen_y < wall_top || screen_y >= wall_bot) {
            continue;
          }
          pixel_buf_[static_cast<std::size_t>(vy * vw + rx)] = grey_px;
        }
      }
    } else {
      // Solid color: orientation and proximity shading as independent multipliers
      const auto base_color = MapUtils::wall_color(ray.wall);
      auto shadow = show_orientation_shading_ ? (ray.x_facing ? MapUtils::orientation_shadow_factor : 1.0f) : 1.0f;
      if (show_proximity_shading_) {
        const auto raw = 1.0f - std::min(max_proximity_shadow, height_mult);
        const auto stepped = float(int(raw / step_size)) * step_size;
        shadow *= stepped;
      }
      const auto sr = std::uint32_t(std::round(float(base_color.r) * shadow));
      const auto sg = std::uint32_t(std::round(float(base_color.g) * shadow));
      const auto sb = std::uint32_t(std::round(float(base_color.b) * shadow));
      const auto solid_px = sr | (sg << 8u) | (sb << 16u) | (0xFFu << 24u);
      for (int vy = vy_min; vy <= vy_max; ++vy) {
        const auto screen_y = (float(vy) + 0.5f) * cell_h_f;
        if (screen_y < wall_top || screen_y >= wall_bot) {
          continue;
        }
        pixel_buf_[static_cast<std::size_t>(vy * vw + rx)] = solid_px;
      }
    }
  }

  // Upload to streaming texture and render
  void *tex_pixels = nullptr;
  int pitch = 0;
  SDL_LockTexture(vscreen_tex_.get(), nullptr, &tex_pixels, &pitch);
  for (int y = 0; y < vh; ++y) {
    std::copy_n(pixel_buf_.data() + y * vw,
                vw,
                reinterpret_cast<std::uint32_t *>(static_cast<std::uint8_t *>(tex_pixels) + y * pitch));
  }
  SDL_UnlockTexture(vscreen_tex_.get());
  SDL_RenderTexture(sdl_r_, vscreen_tex_.get(), nullptr, nullptr);
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
  SDL_RenderDebugText(sdl_r_, tx, ty, std::format("-/=: H-lines    [{}]", k_levels[h_lines_level_].label).c_str());
  ty += line_h;
  SDL_RenderDebugText(sdl_r_, tx, ty, std::format("[/]: Rays       [{}]", k_levels[rays_level_].label).c_str());

  SDL_SetRenderScale(sdl_r_, prev_sx, prev_sy);
}

} // namespace wolf
