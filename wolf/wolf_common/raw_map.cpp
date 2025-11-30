#include "raw_map.hpp"

#include <stdexcept>
#include <type_traits>

namespace wolf {
std::uint16_t RawMap::BlockType::operator[](const std::size_t index) const {
  if (index == 0u) {
    return static_cast<std::underlying_type<Map::Walls>::type>(wall);
  }
  if (index == 1u) {
    return static_cast<std::underlying_type<Map::Objects>::type>(object);
  }
  if (index == 2u) {
    return static_cast<std::underlying_type<Map::Extra>::type>(extra);
  }
  return 0u;
}

std::uint16_t &RawMap::BlockType::operator[](const std::size_t index) {
  if (index == 0u) {
    return reinterpret_cast<std::underlying_type<Map::Walls>::type &>(wall);
  }
  if (index == 1u) {
    return reinterpret_cast<std::underlying_type<Map::Objects>::type &>(object);
  }
  if (index == 2u) {
    return reinterpret_cast<std::underlying_type<Map::Extra>::type &>(extra);
  }
  static auto dump = std::uint16_t{};
  return dump;
}

RawMap::RawMap(const int width, const int height, std::vector<BlockType> &&blocks)
    : size_{width, height}
    , blocks_{std::move(blocks)} {
  process_doors();
  process_ambush_tiles();
  process_start_position();
}

int RawMap::width() const { return size_.x; }

int RawMap::height() const { return size_.y; }

glm::ivec2 RawMap::size() const { return size_; }

glm::ivec2 RawMap::bounds() const { return glm::ivec2{width() - 1, height() - 1}; }

const RawMap::BlockType &RawMap::block(const int w, const int h) const { return block(glm::ivec2{w, h}); }

const RawMap::BlockType &RawMap::block(const glm::ivec2 &pos) const {
  if (!is_within_bounds(pos)) {
    throw std::out_of_range{"RawMap::block(): requested block is out of map bounds"};
  }

  return blocks_[pos.x + pos.y * width()];
}

bool RawMap::is_within_bounds(const int w, const int h) const { return is_within_bounds(glm::ivec2{w, h}); }

bool RawMap::is_within_bounds(const glm::ivec2 &pos) const {
  return pos.x >= 0 && pos.x < width() && pos.y >= 0 && pos.y < height();
}

bool RawMap::is_wall(const int w, const int h) const { return is_wall(glm::ivec2{w, h}); }

bool RawMap::is_wall(const glm::ivec2 &pos) const {
  if (!is_within_bounds(pos)) {
    return false;
  }

  const auto wall = block(pos.x, pos.y).wall;
  return wall > Map::Walls::nothing && wall < Map::Walls::elevator_to_secret_floor;
}

const glm::ivec2 &RawMap::player_pos() const { return player_pos_; }

RawMap::BlockType &RawMap::block(const int w, const int h) {
  if (!is_within_bounds(w, h)) {
    throw std::out_of_range{"RawMap::block(): requested block is out of map bounds"};
  }

  return blocks_[w + h * width()];
}

void RawMap::process_doors() {
  for (auto h = 0; h < height(); ++h) {
    for (auto w = 0; w < width(); ++w) {
      switch (block(w, h).wall) {
      case Map::Walls::door_vertical:
      case Map::Walls::door_vertical_gold_key:
      case Map::Walls::door_vertical_silver_key:
      case Map::Walls::elevator_door_vertical:
        block(w, h).wall = Map::Walls::nothing;
        break;
      case Map::Walls::door_horizontal:
      case Map::Walls::door_horizontal_gold_key:
      case Map::Walls::door_horizontal_silver_key:
      case Map::Walls::elevator_door_horizontal:
        block(w, h).wall = Map::Walls::nothing;
        break;
      default:
        break;
      }
    }
  }
}

void RawMap::process_ambush_tiles() {
  for (auto h = 0; h < height(); ++h) {
    for (auto w = 0; w < width(); ++w) {
      switch (block(w, h).wall) {
      case Map::Walls::floor_deaf_guard:
        block(w, h).wall = Map::Walls::nothing;
        break;
      default:
        break;
      }
    }
  }
}

void RawMap::process_start_position() {
  for (auto h = 0; h < height(); ++h) {
    for (auto w = 0; w < width(); ++w) {
      const auto object = block(w, h).object;
      switch (object) {
      case Map::Objects::start_position_n:
      case Map::Objects::start_position_s:
      case Map::Objects::start_position_w:
      case Map::Objects::start_position_e:
        if (player_pos_ != glm::ivec2{-1, -1}) {
          throw std::runtime_error{"map error: more than one player position detected"};
        }
        player_pos_ = glm::ivec2{w, h};
        break;
      default:
        break;
      }
    }
  }
  if (player_pos_ == glm::ivec2{-1, -1}) {
    throw std::runtime_error{"map error: no player position detected"};
  }
}
} // namespace wolf
