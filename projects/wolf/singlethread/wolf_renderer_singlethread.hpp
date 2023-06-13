#pragma once

#include "projects/wolf/player_state.hpp"
#include "projects/wolf/vector_map.hpp"
#include "projects/wolf/wolf_renderer.hpp"

#include "tools/sdl/sdl_system.hpp"

#include <cstdint>
#include <memory>

namespace wolf {
class WolfRendererSinglethread final : public WolfRenderer {
public:
  WolfRendererSinglethread(const WolfRendererSinglethread &)                = default;
  WolfRendererSinglethread &operator=(const WolfRendererSinglethread &)     = default;
  WolfRendererSinglethread(WolfRendererSinglethread &&) noexcept            = default;
  WolfRendererSinglethread &operator=(WolfRendererSinglethread &&) noexcept = default;
  ~WolfRendererSinglethread() final                                         = default;

  WolfRendererSinglethread(tools::sdl::SDLSystem                   &sdl_sys,
                           const std::shared_ptr<const VectorMap>   vector_map,
                           const std::shared_ptr<const PlayerState> player_state,
                           const std::uint32_t                      rays);

private:
  void prepare_walls() final;
};
} // namespace wolf
