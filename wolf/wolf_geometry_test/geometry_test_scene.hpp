#pragma once

#include "raycaster.hpp"

#include "wolf_common/player_state.hpp"
#include "wolf_common/raw_map.hpp"

#include <gp/sdl/scene_2d.hpp>

#include <glm/mat4x4.hpp>

#include <cstdint>

namespace wolf {
class GeometryTestScene : public gp::sdl::Scene2D {
public:
  GeometryTestScene(std::unique_ptr<const RawMap> raw_map, const std::uint64_t fov_in_degrees);

private:
  void loop(const gp::misc::Event &event) override;

  void initialize(const int width, const int height);
  void finalize();
  void resize(const int width, const int height);
  void redraw();

  void draw_grid(const glm::mat4 &map_mat) const;
  void draw_walls(const glm::mat4 &map_mat) const;
  void draw_colliding_walls(const glm::mat4 &map_mat) const;
  void draw_player(const glm::mat4 &map_mat) const;

  void draw_colored_wall_at(const glm::mat4 &map_mat, const int x, const int y) const;
  void draw_wall_at(const glm::mat4 &map_mat, const int x, const int y) const;

  bool ray_collide(const int wall_x, const int wall_y, const glm::vec2 &player_pos, const glm::vec2 &player_dir) const;

  const std::unique_ptr<const RawMap> raw_map_{};
  PlayerState player_state_{*raw_map_};
  Raycaster raycaster_{*raw_map_, player_state_, 60, 25};
  float width_{};
  float height_{};
  std::uint64_t last_timestamp_ms_{0u};
  float map_scale_{0.9f}; // percent of min(width, height)
};
} // namespace wolf
