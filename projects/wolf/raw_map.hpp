#pragma once

#include "wolf_map_info.hpp"

#include <array>
#include <cstdint>
#include <limits>
#include <tuple>
#include <vector>

namespace wolf {
class RawMap {
public:
  class BlockType {
  public:
    Map::Walls   wall;
    Map::Objects object;
    Map::Extra   extra;

    [[nodiscard]] std::uint16_t  operator[](const std::size_t index) const;
    [[nodiscard]] std::uint16_t &operator[](const std::size_t index);
  };

  RawMap(const RawMap &)                = default;
  RawMap &operator=(const RawMap &)     = default;
  RawMap(RawMap &&) noexcept            = default;
  RawMap &operator=(RawMap &&) noexcept = default;
  ~RawMap()                             = default;

  RawMap(const std::size_t width, const std::size_t height, const std::vector<BlockType> &blocks);
  RawMap(const std::size_t width, const std::size_t height, std::vector<BlockType> &&blocks);

  [[nodiscard]] std::size_t                          width() const;
  [[nodiscard]] std::size_t                          height() const;
  [[nodiscard]] const BlockType                     &block(const std::size_t w, const std::size_t h) const;
  [[nodiscard]] bool                                 is_wall(const std::size_t w, const std::size_t h) const;
  /// Checks if there is a wall on north, south west or east direction from the given location.
  /// @{
  [[nodiscard]] bool                                 is_wall_on_n(const std::size_t w, const std::size_t h) const;
  [[nodiscard]] bool                                 is_wall_on_s(const std::size_t w, const std::size_t h) const;
  [[nodiscard]] bool                                 is_wall_on_w(const std::size_t w, const std::size_t h) const;
  [[nodiscard]] bool                                 is_wall_on_e(const std::size_t w, const std::size_t h) const;
  /// @}
  [[nodiscard]] std::tuple<std::size_t, std::size_t> player_pos() const;

private:
  std::size_t            width_{};
  std::size_t            height_{};
  std::vector<BlockType> blocks_{};
  std::size_t            player_pos_w_{std::numeric_limits<std::size_t>::max()};
  std::size_t            player_pos_h_{std::numeric_limits<std::size_t>::max()};

  [[nodiscard]] BlockType &block(const std::size_t w, const std::size_t h);

  void process_doors();
  void process_ambush_tiles();
  void process_start_position();
};
} // namespace wolf
