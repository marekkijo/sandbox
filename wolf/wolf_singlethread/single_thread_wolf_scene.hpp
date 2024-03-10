#pragma once

#include "wolf_common/wolf_scene.hpp"

#include <cstdint>

namespace wolf {
class SingleThreadWolfScene final : public WolfScene {
public:
  SingleThreadWolfScene(const RawMap &raw_map, const std::uint32_t fov_in_degrees, const std::uint32_t number_of_rays);
  ~SingleThreadWolfScene() final = default;

  SingleThreadWolfScene(const SingleThreadWolfScene &) = delete;
  SingleThreadWolfScene &operator=(const SingleThreadWolfScene &) = delete;
  SingleThreadWolfScene(SingleThreadWolfScene &&) noexcept = delete;
  SingleThreadWolfScene &operator=(SingleThreadWolfScene &&) noexcept = delete;

private:
  void prepare_walls() final;
};
} // namespace wolf
