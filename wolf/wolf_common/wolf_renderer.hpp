#pragma once

#include "player_state.hpp"
#include "renderer.hpp"
#include "vector_map.hpp"

#include <gp/sdl/system.hpp>

#include <SDL2/SDL_rect.h>

#include <cstdint>
#include <memory>
#include <vector>

namespace wolf {
class WolfRenderer : public Renderer {
public:
  WolfRenderer(const WolfRenderer &) = default;
  WolfRenderer &operator=(const WolfRenderer &) = default;
  WolfRenderer(WolfRenderer &&) noexcept = default;
  WolfRenderer &operator=(WolfRenderer &&) noexcept = default;
  ~WolfRenderer() = default;

  WolfRenderer(gp::sdl::System &sdl_sys,
               const std::shared_ptr<const VectorMap> vector_map,
               const std::shared_ptr<const PlayerState> player_state,
               const std::uint32_t rays);

  void redraw() final;

public:
  struct Wall {
    SDL_Rect rect;
    std::size_t color_index;
    float shadow_factor;
  };

  struct RayRot {
    float sin;
    float cos;
  };

protected:
  const std::shared_ptr<const VectorMap> vector_map_{};
  const std::shared_ptr<const PlayerState> player_state_{};

  std::vector<Wall> walls_{};
  std::vector<RayRot> ray_rots_{};
  int width_{};
  int height_{};

  virtual void prepare_walls() = 0;

private:
  void resize(const int width, const int height) final;

  void draw_background() const;
  void draw_walls() const;
};
} // namespace wolf
