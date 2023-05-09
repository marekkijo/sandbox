#pragma once

#include "raw_map.hpp"

#include <array>
#include <vector>

namespace wolf {
class VectorMap {
public:
  VectorMap() = delete;
  VectorMap(const VectorMap &) = default;
  VectorMap &operator=(const VectorMap &) = default;
  VectorMap(VectorMap &&) noexcept = default;
  VectorMap &operator=(VectorMap &&) noexcept = default;
  ~VectorMap() = default;

  VectorMap(const RawMap &raw_map);

  [[nodiscard]] const std::vector<std::array<float, 4>> &vectors() const;
  [[nodiscard]] const std::vector<char> &colors() const;

private:
  std::vector<std::array<float, 4>> vectors_{};
  std::vector<char> colors_{};
};
} // namespace wolf
