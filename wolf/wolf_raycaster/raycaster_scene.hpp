#pragma once

#include "raycaster.hpp"

#include "wolf_common/map_renderer.hpp"
#include "wolf_common/player_state.hpp"
#include "wolf_common/raw_map.hpp"
#include "wolf_common/vector_map.hpp"
#include "wolf_common/vswap_file.hpp"

#include <gp/sdl/scene_2d.hpp>
#include <gp/sdl/sdl.hpp>

#include <memory>
#include <vector>

namespace wolf {
class RaycasterScene : public gp::sdl::Scene2D {
public:
  RaycasterScene(std::unique_ptr<const RawMap> raw_map,
                 std::shared_ptr<const VswapFile> vswap_file,
                 const int fov_in_degrees,
                 const int num_rays);

private:
  void loop(const gp::misc::Event &event) override;

  void resize(const int width, const int height);
  void redraw();

  void init_wall_textures();
  void draw_background() const;
  void draw_walls() const;
  void draw_help_overlay() const;

  const std::unique_ptr<const RawMap> raw_map_{};
  std::shared_ptr<const VswapFile> vswap_file_{};
  PlayerState player_state_{*raw_map_};
  VectorMap vector_map_{*raw_map_};
  Raycaster raycaster_;
  MapRenderer map_renderer_;
  int width_{};
  int height_{};
  std::uint32_t last_timestamp_ms_{0u};

  bool show_textures_{true};
  bool show_shading_{true};
  bool show_map_{true};
  bool map_player_oriented_{false};

  SDL_Renderer *sdl_r_{};
  std::vector<std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)>> wall_textures_;
};
} // namespace wolf
