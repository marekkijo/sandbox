#pragma once

#include "wolf_common/map_renderer.hpp"
#include "wolf_common/player_state.hpp"
#include "wolf_common/raw_map.hpp"
#include "wolf_common/vector_map.hpp"

#include <gp/sdl/scene_2d.hpp>

#include <SDL2/SDL_rect.h>

#include <cstdint>
#include <memory>
#include <vector>

namespace wolf {
class WolfScene : public gp::sdl::Scene2D {
public:
  WolfScene(const RawMap &raw_map, const std::uint32_t fov_in_degrees, const std::uint32_t number_of_rays);
  ~WolfScene() override = default;

  WolfScene(const WolfScene &) = delete;
  WolfScene &operator=(const WolfScene &) = delete;
  WolfScene(WolfScene &&) noexcept = delete;
  WolfScene &operator=(WolfScene &&) noexcept = delete;

protected:
  struct RayRot {
    float sin;
    float cos;
  };

  struct Wall {
    SDL_Rect rect;
    std::size_t color_index;
    float shadow_factor;
  };

  virtual void prepare_walls() = 0;

  std::shared_ptr<VectorMap> vector_map_{};
  std::shared_ptr<PlayerState> player_state_{};
  std::vector<RayRot> ray_rots_{};
  std::vector<Wall> walls_{};
  int width_{};
  int height_{};

private:
  void loop(const gp::misc::Event &event) final;

  void initialize(const int width, const int height);
  void finalize();
  void resize(const int width, const int height);
  void animate(const std::uint32_t time_elapsed_ms);
  void redraw();

  void draw_background() const;
  void draw_walls() const;

  std::unique_ptr<MapRenderer> map_renderer_{};
  std::uint32_t last_timestamp_ms_{};
};
} // namespace wolf
