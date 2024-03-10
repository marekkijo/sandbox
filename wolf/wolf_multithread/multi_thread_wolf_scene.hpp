#pragma once

#include "wolf_common/wolf_scene.hpp"

#include <cstdint>
#include <mutex>
#include <thread>
#include <vector>

namespace wolf {
class MultiThreadWolfScene final : public WolfScene {
public:
  MultiThreadWolfScene(const RawMap &raw_map,
                       const std::uint32_t fov_in_degrees,
                       const std::uint32_t number_of_rays,
                       const std::uint32_t number_of_threads);
  ~MultiThreadWolfScene() final;

  MultiThreadWolfScene(const MultiThreadWolfScene &) = delete;
  MultiThreadWolfScene &operator=(const MultiThreadWolfScene &) = delete;
  MultiThreadWolfScene(MultiThreadWolfScene &&) noexcept = delete;
  MultiThreadWolfScene &operator=(MultiThreadWolfScene &&) noexcept = delete;

private:
  struct PrepareWallsThreadState;
  class PrepareWallsThreadCallable;
  friend class PrepareWallsThreadCallable;

  void prepare_walls() final;

  std::unique_ptr<PrepareWallsThreadState> state_;
  std::vector<std::thread> threads_{};
};
} // namespace wolf
