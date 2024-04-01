#pragma once

#include <glm/glm.hpp>

#include <array>
#include <string>
#include <vector>

namespace wolf {
class VswapFile {
public:
  VswapFile(const VswapFile &) = default;
  VswapFile &operator=(const VswapFile &) = default;
  VswapFile(VswapFile &&) noexcept = default;
  VswapFile &operator=(VswapFile &&) noexcept = default;
  ~VswapFile() = default;

  VswapFile(const std::string &vswap_filename);

  [[nodiscard]] const std::vector<std::array<glm::u8vec4, 4096>> &walls() const noexcept;
  [[nodiscard]] const std::vector<std::array<glm::u8vec4, 4096>> &sprites() const noexcept;
  [[nodiscard]] const std::vector<std::array<bool, 4096>> &sprites_opaque() const noexcept;

private:
  void decode_walls(std::ifstream &vswap_file, const std::vector<std::uint32_t> &chunk_offsets);
  void decode_sprites(std::ifstream &vswap_file,
                      const std::vector<std::uint32_t> &chunk_offsets,
                      const std::vector<std::uint16_t> &chunk_lengths);

  std::vector<std::array<glm::u8vec4, 4096>> walls_;
  std::vector<std::array<glm::u8vec4, 4096>> sprites_;
  /**
   * The opaque of each pixel in the sprites.
   * true if the pixel is opaque, false otherwise (transparent)
   */
  std::vector<std::array<bool, 4096>> sprites_opaque_;
};
} // namespace wolf
