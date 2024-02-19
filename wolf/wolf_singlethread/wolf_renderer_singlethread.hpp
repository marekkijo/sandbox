#pragma once

#include "wolf_common/player_state.hpp"
#include "wolf_common/vector_map.hpp"
#include "wolf_common/wolf_renderer.hpp"

#include <gp/sdl/system.hpp>

#include <cstdint>
#include <memory>

namespace wolf {
class WolfRendererSinglethread final : public WolfRenderer {
public:
  WolfRendererSinglethread(const WolfRendererSinglethread &) = default;
  WolfRendererSinglethread &operator=(const WolfRendererSinglethread &) = default;
  WolfRendererSinglethread(WolfRendererSinglethread &&) noexcept = default;
  WolfRendererSinglethread &operator=(WolfRendererSinglethread &&) noexcept = default;
  ~WolfRendererSinglethread() final = default;

  WolfRendererSinglethread(gp::sdl::System &sdl_sys,
                           const std::shared_ptr<const VectorMap> vector_map,
                           const std::shared_ptr<const PlayerState> player_state,
                           const std::uint32_t rays);

private:
  void prepare_walls() final;
};
} // namespace wolf
