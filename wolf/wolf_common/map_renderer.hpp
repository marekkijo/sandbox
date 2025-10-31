#pragma once

#include "wolf_common/player_state.hpp"
#include "wolf_common/vector_map.hpp"

#include <gp/sdl/renderer.hpp>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include <memory>

namespace wolf {
class MapRenderer {
public:
  MapRenderer(const VectorMap &vector_map,
              const PlayerState &player_state,
              const std::uint32_t fov_in_degrees,
              const bool player_oriented = false);

  void set_renderer(std::shared_ptr<const gp::sdl::Renderer> renderer);
  void resize(const int width, const int height);
  void redraw();

private:
  const gp::sdl::Renderer &r() const;
  void redraw_player_oriented() const;
  void redraw_map_oriented() const;
  void draw_map(const glm::mat4 &mat) const;
  void draw_player(const glm::mat4 &mat) const;

  const VectorMap &vector_map_;
  const PlayerState &player_state_;
  const float fov_in_rad_{};
  const bool player_oriented_{};

  std::shared_ptr<const gp::sdl::Renderer> r_{};

  float scale_{};
  glm::vec3 screen_center_translation_{};
};
} // namespace wolf
