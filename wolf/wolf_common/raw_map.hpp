#pragma once

#include "wolf_common/wolf_map_info.hpp"

#include <glm/vec2.hpp>

#include <cstdint>
#include <vector>

namespace wolf {
class RawMap {
public:
  class BlockType {
  public:
    Map::Walls wall;
    Map::Objects object;
    Map::Extra extra;

    std::uint16_t operator[](const std::size_t index) const;
    std::uint16_t &operator[](const std::size_t index);
  };

  RawMap(const int width, const int height, std::vector<BlockType> &&blocks);

  int width() const;
  int height() const;
  glm::ivec2 size() const;
  glm::ivec2 bounds() const;
  const BlockType &block(const int w, const int h) const;
  bool is_within_bounds(const int w, const int h) const;
  bool is_wall(const int w, const int h) const;

  const glm::ivec2 &player_pos() const;

private:
  glm::ivec2 size_{-1, -1};

  std::vector<BlockType> blocks_{};
  glm::ivec2 player_pos_{-1, -1};

  BlockType &block(const int w, const int h);

  void process_doors();
  void process_ambush_tiles();
  void process_start_position();
};
} // namespace wolf
