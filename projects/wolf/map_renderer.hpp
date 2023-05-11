#pragma once

#include "player_state.hpp"
#include "renderer.hpp"
#include "vector_map.hpp"

#include "tools/sdl/sdl_system.hpp"

#include <glm/mat4x4.hpp>

#include <memory>
#include <tuple>

namespace wolf {
class MapRenderer final : public Renderer {
public:
  MapRenderer(const MapRenderer &) = default;
  MapRenderer &operator=(const MapRenderer &) = default;
  MapRenderer(MapRenderer &&) noexcept = default;
  MapRenderer &operator=(MapRenderer &&) noexcept = default;
  ~MapRenderer() final = default;

  MapRenderer(tools::sdl::SDLSystem &sdl_sys, std::shared_ptr<const VectorMap> vector_map,
              std::shared_ptr<const PlayerState> player_state);

  void redraw() final;

protected:
  void resize(int width, int height) final;

private:
  std::shared_ptr<const VectorMap> vector_map_{};
  std::shared_ptr<const PlayerState> player_state_{};

  bool player_oriented_{false};
  float translate_x_{};
  float translate_y_{};
  float scale_{};

  [[nodiscard]] const VectorMap &vector_map() const;
  [[nodiscard]] const PlayerState &player_state() const;
  [[nodiscard]] bool player_oriented() const;
  [[nodiscard]] float translate_x() const;
  [[nodiscard]] float translate_y() const;
  [[nodiscard]] std::tuple<float, float> translate() const;
  [[nodiscard]] float scale() const;
  void redraw_player_oriented() const;
  void redraw_map_oriented() const;
  void draw_map(const glm::mat4 &mat) const;
  void draw_player(const glm::mat4 &mat) const;
};
} // namespace wolf
