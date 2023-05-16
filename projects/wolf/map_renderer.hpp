#pragma once

#include "player_state.hpp"
#include "renderer.hpp"
#include "vector_map.hpp"

#include "tools/sdl/sdl_system.hpp"

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include <memory>

namespace wolf {
class MapRenderer final : public Renderer {
public:
  MapRenderer(const MapRenderer &)                = default;
  MapRenderer &operator=(const MapRenderer &)     = default;
  MapRenderer(MapRenderer &&) noexcept            = default;
  MapRenderer &operator=(MapRenderer &&) noexcept = default;
  ~MapRenderer() final                            = default;

  MapRenderer(tools::sdl::SDLSystem             &sdl_sys,
              std::shared_ptr<const VectorMap>   vector_map,
              std::shared_ptr<const PlayerState> player_state);

  void redraw() final;

protected:
  void resize(int width, int height) final;

private:
  std::shared_ptr<const VectorMap>   vector_map_{};
  std::shared_ptr<const PlayerState> player_state_{};

  bool      player_oriented_{false};
  float     scale_{};
  glm::vec3 screen_center_translation_{};

  void redraw_player_oriented() const;
  void redraw_map_oriented() const;
  void draw_map(const glm::mat4 &mat) const;
  void draw_player(const glm::mat4 &mat) const;
};
} // namespace wolf
