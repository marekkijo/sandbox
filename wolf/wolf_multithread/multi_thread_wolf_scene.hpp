#pragma once

#include "wolf_common/wolf_scene.hpp"

#include <cstdint>
#include <thread>
#include <vector>

namespace wolf {
class MultiThreadWolfScene : public WolfScene {
public:
  MultiThreadWolfScene(std::unique_ptr<const RawMap> raw_map,
                       const std::uint32_t fov_in_degrees,
                       const std::uint32_t number_of_rays,
                       const std::uint32_t number_of_threads);
  ~MultiThreadWolfScene() override;

private:
  struct PrepareWallsThreadState;
  class PrepareWallsThreadCallable;
  friend class PrepareWallsThreadCallable;

  void prepare_walls() override;

  std::unique_ptr<PrepareWallsThreadState> state_;
  std::vector<std::thread> threads_{};
};
} // namespace wolf
