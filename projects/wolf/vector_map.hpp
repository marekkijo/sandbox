#pragma once

#include "raw_map.hpp"

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include <array>
#include <memory>
#include <utility>
#include <vector>

namespace wolf
{
  class VectorMap
  {
  public:
    VectorMap(const VectorMap &) = default;
    VectorMap &operator=(const VectorMap &) = default;
    VectorMap(VectorMap &&) noexcept = default;
    VectorMap &operator=(VectorMap &&) noexcept = default;
    ~VectorMap() = default;

    VectorMap(std::shared_ptr<const RawMap> raw_map);

    [[nodiscard]] const std::vector<std::pair<glm::vec2, glm::vec2>> &vectors() const;
    [[nodiscard]] glm::uvec3 color(std::size_t index, float shadow_factor = 1.0f) const;
    [[nodiscard]] float width() const;
    [[nodiscard]] float height() const;
    [[nodiscard]] float diagonal_length() const;

  private:
    const std::shared_ptr<const RawMap> raw_map_{};
    std::vector<std::pair<glm::vec2, glm::vec2>> vectors_{};
    std::vector<char> colors_{};
    float diagonal_length_{};

    void generate_vector_map();
  };
} // namespace wolf
