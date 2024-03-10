#pragma once

#include "wolf_common/raw_map.hpp"
#include "wolf_common/wolf_map_info.hpp"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <cstdint>
#include <utility>
#include <vector>

namespace wolf {
class VectorMap {
public:
  explicit VectorMap(const RawMap &raw_map);

  VectorMap(const VectorMap &) = default;
  VectorMap &operator=(const VectorMap &) = default;
  VectorMap(VectorMap &&) noexcept = default;
  VectorMap &operator=(VectorMap &&) noexcept = default;
  ~VectorMap() = default;

  [[nodiscard]] const std::vector<std::pair<glm::vec2, glm::vec2>> &vectors() const;
  [[nodiscard]] const std::vector<glm::uvec3> &colors() const;
  [[nodiscard]] glm::uvec3 color(const std::size_t index, const float shadow_factor = 1.0f) const;
  [[nodiscard]] float width() const;
  [[nodiscard]] float height() const;
  [[nodiscard]] float diagonal_length() const;

private:
  const float width_{};
  const float height_{};
  const float diagonal_length_{};

  std::vector<std::pair<glm::vec2, glm::vec2>> vectors_{};
  std::vector<glm::uvec3> colors_{};

  void generate_vector_map(const RawMap &raw_map);
};
} // namespace wolf
