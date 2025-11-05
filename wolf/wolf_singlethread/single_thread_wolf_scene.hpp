#pragma once

#include "wolf_common/wolf_scene.hpp"

#include <cstdint>
#include <memory>

namespace wolf {
class SingleThreadWolfScene : public WolfScene {
public:
  SingleThreadWolfScene(std::unique_ptr<const RawMap> raw_map,
                        const std::uint32_t fov_in_degrees,
                        const std::uint32_t number_of_rays);

private:
  void prepare_walls() override;
};
} // namespace wolf
