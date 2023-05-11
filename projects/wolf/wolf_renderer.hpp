#pragma once

#include "player_state.hpp"
#include "renderer.hpp"
#include "vector_map.hpp"

#include "tools/sdl/sdl_system.hpp"

#include <SDL2/SDL_rect.h>

#include <cstdint>
#include <memory>
#include <vector>

namespace wolf {
class WolfRenderer final : public Renderer {
public:
  WolfRenderer(const WolfRenderer &) = default;
  WolfRenderer &operator=(const WolfRenderer &) = default;
  WolfRenderer(WolfRenderer &&) noexcept = default;
  WolfRenderer &operator=(WolfRenderer &&) noexcept = default;
  ~WolfRenderer() final = default;

  WolfRenderer(tools::sdl::SDLSystem &sdl_sys, std::shared_ptr<const VectorMap> vector_map,
               std::shared_ptr<const PlayerState> player_state, std::uint32_t res_w = 64u, std::uint32_t res_h = 64u);

  void redraw() final;

protected:
  void resize(int width, int height) final;

private:
  struct RayRot {
    float sin;
    float cos;
  };
  struct WallHeight {
    int y;
    int h;
  };
  struct Wall {
    SDL_Rect rect;
    std::size_t color_index;
    float shadow_factor;
  };

  std::shared_ptr<const VectorMap> vector_map_{};
  std::shared_ptr<const PlayerState> player_state_{};

  std::vector<Wall> walls_{};
  std::vector<RayRot> ray_rots_{};
  std::uint32_t res_h_{};
  std::vector<WallHeight> wall_heights_{};
  SDL_Rect ceiling_rect_{};
  SDL_Rect floor_rect_{};

  [[nodiscard]] const VectorMap &vector_map() const;
  [[nodiscard]] const PlayerState &player_state() const;
  [[nodiscard]] const std::vector<RayRot> &ray_rots() const;
  [[nodiscard]] std::uint32_t res_h() const;
  [[nodiscard]] const std::vector<WallHeight> &wall_heights() const;
  [[nodiscard]] const WallHeight &wall_height(float factor) const;
  [[nodiscard]] const SDL_Rect &ceiling_rect() const;
  [[nodiscard]] const SDL_Rect &floor_rect() const;
  void prepare_walls();
  void draw_background() const;
  void draw_walls() const;
};
} // namespace wolf
