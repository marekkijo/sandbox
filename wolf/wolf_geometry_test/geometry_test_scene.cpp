#include "geometry_test_scene.hpp"

#include "wolf_common/map_utils.hpp"

#include <glm/fwd.hpp>
#include <gp/math/math.hpp>

#include <glm/ext/matrix_transform.hpp>

namespace wolf {
namespace {
bool ray_aabb_intersect_2d(const glm::vec2 &ray_start,
                           const glm::vec2 &ray_dir,
                           const glm::vec2 &aabb_min,
                           const glm::vec2 &aabb_max) {
  const auto inv_dir = 1.0f / ray_dir;

  const auto t1 = (aabb_min.x - ray_start.x) * inv_dir.x;
  const auto t2 = (aabb_max.x - ray_start.x) * inv_dir.x;
  const auto t3 = (aabb_min.y - ray_start.y) * inv_dir.y;
  const auto t4 = (aabb_max.y - ray_start.y) * inv_dir.y;

  const auto tmin = std::max(std::min(t1, t2), std::min(t3, t4));
  const auto tmax = std::min(std::max(t1, t2), std::max(t3, t4));

  if (tmax < 0.0f) {
    return false; // AABB is behind the ray
  }

  if (tmin > tmax) {
    return false; // No intersection
  }

  return true;
}

bool ray_collide(const int wall_w, const int wall_h, const glm::vec2 &ray_start, const glm::vec2 &ray_dir) {
  const auto wall_min = glm::vec2{wall_w, wall_h};
  const auto wall_max = wall_min + glm::vec2{1.0f, 1.0f};
  return ray_aabb_intersect_2d(ray_start, ray_dir, wall_min, wall_max);
}
} // namespace

GeometryTestScene::GeometryTestScene(std::unique_ptr<const RawMap> raw_map, const std::uint32_t fov_in_degrees)
    : raw_map_{std::move(raw_map)}
    , last_timestamp_ms_{timestamp()} {}

void GeometryTestScene::loop(const gp::misc::Event &event) {
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
    player_state_.animate(time_elapsed_ms);
    last_timestamp_ms_ = event.timestamp();
    redraw();
  } break;
  default:
    break;
  }
}

void GeometryTestScene::initialize(const int width, const int height) {
  player_state_.set_keyboard_state(keyboard_state());
  resize(width, height);
}

void GeometryTestScene::finalize() { player_state_.set_keyboard_state(nullptr); }

void GeometryTestScene::resize(const int width, const int height) {
  width_ = width;
  height_ = height;
}

void GeometryTestScene::redraw() {
  r().set_color(24, 24, 24);
  r().clear();

  auto map_mat = glm::mat4(1.0f);

  // Flip the y-axis
  constexpr auto flip_y = -1.0f;

  // Translate to the center of the screen
  const auto screen_min = static_cast<float>(std::min(width_, height_));
  const auto map_max = screen_min * map_scale_;
  const auto screen_center_translation = glm::vec3{width_ - map_max, height_ + (map_max * flip_y), 0.0f} / 2.0f;
  map_mat = glm::translate(map_mat, screen_center_translation);

  // Scale to fit the screen
  const auto map_min = static_cast<float>(std::min(raw_map_->width(), raw_map_->height()));
  const auto scale_factor = (screen_min / map_min) * map_scale_;
  map_mat = glm::scale(map_mat, glm::vec3{scale_factor, -scale_factor * flip_y, 1.0f});

  draw_grid(map_mat);
  draw_walls(map_mat);
  draw_colliding_walls(map_mat);
  draw_player(map_mat);

  r().present();
}

void GeometryTestScene::draw_grid(const glm::mat4 &map_mat) const {
  const auto pt_00 = map_mat * glm::vec4{0.0f, 0.0f, 0.0f, 1.0f};
  const auto pt_w0 = map_mat * glm::vec4{raw_map_->width(), 0.0f, 0.0f, 1.0f};
  const auto pt_0h = map_mat * glm::vec4{0.0f, raw_map_->height(), 0.0f, 1.0f};
  const auto pt_wh = map_mat * glm::vec4{raw_map_->width(), raw_map_->height(), 0.0f, 1.0f};

  r().set_color(32, 32, 32);
  for (int x = 1; x < raw_map_->width(); ++x) {
    const auto pt_x0 = map_mat * glm::vec4{x, 0.0f, 0.0f, 1.0f};
    const auto pt_xh = map_mat * glm::vec4{x, raw_map_->height(), 0.0f, 1.0f};
    r().draw_line(pt_x0, pt_xh);
  }
  for (int y = 1; y < raw_map_->height(); ++y) {
    const auto pt_0y = map_mat * glm::vec4{0.0f, y, 0.0f, 1.0f};
    const auto pt_wy = map_mat * glm::vec4{raw_map_->width(), y, 0.0f, 1.0f};
    r().draw_line(pt_0y, pt_wy);
  }

  r().set_color(255, 255, 255);
  r().draw_line(pt_00, pt_0h);
  r().draw_line(pt_wh, pt_w0);
  r().draw_line(pt_0h, pt_wh);
  r().draw_line(pt_w0, pt_00);
}

void GeometryTestScene::draw_walls(const glm::mat4 &map_mat) const {
  for (int x = 0; x < raw_map_->width(); ++x) {
    for (int y = 0; y < raw_map_->height(); ++y) {
      if (raw_map_->is_wall(x, y)) {
        draw_colored_wall_at(map_mat, x, y);
      }
    }
  }
}

void GeometryTestScene::draw_colliding_walls(const glm::mat4 &map_mat) const {
  const auto player_pos = player_state_.pos();
  const auto player_dir = player_state_.dir();
  const auto player_block = glm::min(glm::ivec2{player_pos}, raw_map_->bounds());
  const auto search_dir = glm::ivec2{
      player_dir.x > 0.0f ? 1 : -1,
      player_dir.y > 0.0f ? 1 : -1,
  };

  for (auto depth = 1; depth < std::max(raw_map_->width(), raw_map_->height()); ++depth) {
    const auto max_x = player_block.x + search_dir.x * depth;
    const auto min_x = player_block.x + search_dir.x * 0;
    const auto max_y = player_block.y + search_dir.y * depth;
    const auto min_y = player_block.y + search_dir.y * 0;

    if (!raw_map_->is_within_bounds(max_x, min_y) && !raw_map_->is_within_bounds(min_x, max_y)) {
      break; // early exit: out of bounds
    }

    for (auto y_depth = 0; y_depth <= depth; ++y_depth) {
      const auto curr_y = min_y + search_dir.y * y_depth;
      if (raw_map_->is_wall(max_x, curr_y)) {
        r().set_color(255, 255, 128, 32);
        draw_wall_at(map_mat, max_x, curr_y);
        if (ray_collide(max_x, curr_y, player_pos, player_dir)) {
          r().set_color(128, 128, 255, 128);
          draw_wall_at(map_mat, max_x, curr_y);
        }
      }
    }

    for (auto x_depth = depth - 1; x_depth >= 0; --x_depth) {
      const auto curr_x = min_x + search_dir.x * x_depth;
      if (raw_map_->is_wall(curr_x, max_y)) {
        r().set_color(255, 255, 128, 32);
        draw_wall_at(map_mat, curr_x, max_y);
        if (ray_collide(curr_x, max_y, player_pos, player_dir)) {
          r().set_color(128, 128, 255, 128);
          draw_wall_at(map_mat, curr_x, max_y);
        }
      }
    }
  }
}

void GeometryTestScene::draw_player(const glm::mat4 &map_mat) const {
  const auto player_pos = player_state_.pos();
  const auto player_dir = player_state_.dir();
  const auto player_dist = player_pos + player_dir * 32.0f;
  const auto screen_player_pos = map_mat * glm::vec4{player_pos, 0.0f, 1.0f};
  const auto screen_player_dist = map_mat * glm::vec4{player_dist, 0.0f, 1.0f};

  r().draw_gradient_line(screen_player_pos,
                         glm::uvec4(255u, 0u, 0u, 255u),
                         screen_player_dist,
                         glm::uvec4(0u, 0u, 0u, 0u));

  const auto player_block = glm::min(glm::ivec2{player_pos}, raw_map_->bounds());
  r().set_color(255u, 0u, 0u, 128u);
  draw_wall_at(map_mat, player_block.x, player_block.y);
}

void GeometryTestScene::draw_colored_wall_at(const glm::mat4 &map_mat, const int x, const int y) const {
  const auto wall_color = MapUtils::wall_color(raw_map_->block(x, y).wall);
  r().set_color(glm::uvec4{wall_color, 32u});
  draw_wall_at(map_mat, x, y);
}

void GeometryTestScene::draw_wall_at(const glm::mat4 &map_mat, const int x, const int y) const {
  constexpr auto wall_size = 0.9f;

  const auto pt_tl = map_mat * glm::vec4{x, y, 0.0f, 1.0f};
  const auto pt_br = map_mat * glm::vec4{x + wall_size, y + wall_size, 0.0f, 1.0f};
  const SDL_Rect wall_rect{static_cast<int>(pt_tl.x),
                           static_cast<int>(pt_tl.y),
                           static_cast<int>(pt_br.x - pt_tl.x),
                           static_cast<int>(pt_br.y - pt_tl.y)};
  r().fill_rect(wall_rect);
}
} // namespace wolf
