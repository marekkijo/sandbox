#pragma once

#include "projects/wolf/raw_map.hpp"
#include "projects/wolf/wolf_map_info.hpp"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

namespace wolf {
class VectorMap {
public:
  VectorMap(const VectorMap &)                = default;
  VectorMap &operator=(const VectorMap &)     = default;
  VectorMap(VectorMap &&) noexcept            = default;
  VectorMap &operator=(VectorMap &&) noexcept = default;
  ~VectorMap()                                = default;

  VectorMap(const std::shared_ptr<const RawMap> raw_map);

  [[nodiscard]] const std::vector<std::pair<glm::vec2, glm::vec2>> &vectors() const;
  [[nodiscard]] const std::vector<glm::uvec3>                      &colors() const;
  [[nodiscard]] glm::uvec3 color(const std::size_t index, const float shadow_factor = 1.0f) const;
  [[nodiscard]] float      width() const;
  [[nodiscard]] float      height() const;
  [[nodiscard]] float      diagonal_length() const;

private:
  const std::shared_ptr<const RawMap> raw_map_{};

  std::vector<std::pair<glm::vec2, glm::vec2>> vectors_{};
  std::vector<glm::uvec3>                      colors_{};
  float                                        diagonal_length_{};

  void                            generate_vector_map();
  [[nodiscard]] static glm::uvec3 wall_color(const Map::Walls wall);
};
} // namespace wolf
