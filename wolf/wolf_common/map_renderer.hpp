#pragma once

#include "player_state.hpp"
#include "renderer.hpp"
#include "vector_map.hpp"

#include <gp/sdl/system.hpp>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include <memory>

namespace wolf {
class MapRenderer final : public Renderer {
public:
  MapRenderer(const MapRenderer &) = default;
  MapRenderer &operator=(const MapRenderer &) = default;
  MapRenderer(MapRenderer &&) noexcept = default;
  MapRenderer &operator=(MapRenderer &&) noexcept = default;
  ~MapRenderer() final = default;

  MapRenderer(gp::sdl::System &sdl_sys,
              const std::shared_ptr<const VectorMap> vector_map,
              const std::shared_ptr<const PlayerState> player_state,
              const bool player_oriented = false);

  void redraw() final;

protected:
  void resize(const int width, const int height) final;

private:
  const std::shared_ptr<const VectorMap> vector_map_{};
  const std::shared_ptr<const PlayerState> player_state_{};
  const bool player_oriented_{};

  float scale_{};
  glm::vec3 screen_center_translation_{};

  void redraw_player_oriented() const;
  void redraw_map_oriented() const;
  void draw_map(const glm::mat4 &mat) const;
  void draw_player(const glm::mat4 &mat) const;
};
} // namespace wolf
