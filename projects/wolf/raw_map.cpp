#include "projects/wolf/raw_map.hpp"

#include "projects/wolf/wolf_map_info.hpp"

#include <stdexcept>
#include <type_traits>

namespace wolf {
std::uint16_t RawMap::BlockType::operator[](const std::size_t index) const {
  if (index == 0u) { return static_cast<std::underlying_type<Map::Walls>::type>(wall); }
  if (index == 1u) { return static_cast<std::underlying_type<Map::Objects>::type>(object); }
  if (index == 2u) { return static_cast<std::underlying_type<Map::Extra>::type>(extra); }
  return 0u;
}

std::uint16_t &RawMap::BlockType::operator[](const std::size_t index) {
  if (index == 0u) { return reinterpret_cast<std::underlying_type<Map::Walls>::type &>(wall); }
  if (index == 1u) { return reinterpret_cast<std::underlying_type<Map::Objects>::type &>(object); }
  if (index == 2u) { return reinterpret_cast<std::underlying_type<Map::Extra>::type &>(extra); }
  static auto dump = std::uint16_t{};
  return dump;
}

RawMap::RawMap(const std::size_t width, const std::size_t height, std::vector<BlockType> &&blocks)
    : width_{width}, height_{height}, blocks_{std::move(blocks)} {
  process_doors();
  process_ambush_tiles();
  process_start_position();
}

std::size_t RawMap::width() const { return width_; }

std::size_t RawMap::height() const { return height_; }

const RawMap::BlockType &RawMap::block(const std::size_t w, const std::size_t h) const {
  return blocks_[w + h * width()];
}

bool RawMap::is_wall(const std::size_t w, const std::size_t h) const {
  const auto wall = block(w, h).wall;
  return wall > Map::Walls::nothing && wall < Map::Walls::elevator_to_secret_floor;
}

bool RawMap::is_wall_on_n(const std::size_t w, const std::size_t h) const { return h == 0 || is_wall(w, h - 1); }

bool RawMap::is_wall_on_s(const std::size_t w, const std::size_t h) const {
  return h == (height() - 1) || is_wall(w, h + 1);
}

bool RawMap::is_wall_on_w(const std::size_t w, const std::size_t h) const { return w == 0 || is_wall(w - 1, h); }

bool RawMap::is_wall_on_e(const std::size_t w, const std::size_t h) const {
  return w == (width() - 1) || is_wall(w + 1, h);
}

std::tuple<std::size_t, std::size_t> RawMap::player_pos() const {
  return std::make_tuple(player_pos_w_, player_pos_h_);
}

RawMap::BlockType &RawMap::block(const std::size_t w, const std::size_t h) { return blocks_[w + h * width()]; }

void RawMap::process_doors() {
  for (auto h = std::size_t{0u}; h < height(); h++) {
    for (auto w = std::size_t{0u}; w < width(); w++) {
      switch (block(w, h).wall) {
      case Map::Walls::door_vertical:
      case Map::Walls::door_vertical_gold_key:
      case Map::Walls::door_vertical_silver_key:
      case Map::Walls::elevator_door_vertical: block(w, h).wall = Map::Walls::nothing; break;
      case Map::Walls::door_horizontal:
      case Map::Walls::door_horizontal_gold_key:
      case Map::Walls::door_horizontal_silver_key:
      case Map::Walls::elevator_door_horizontal: block(w, h).wall = Map::Walls::nothing; break;
      default: break;
      }
    }
  }
}

void RawMap::process_ambush_tiles() {
  for (auto h = std::size_t{0u}; h < height(); h++) {
    for (auto w = std::size_t{0u}; w < width(); w++) {
      switch (block(w, h).wall) {
      case Map::Walls::floor_deaf_guard: block(w, h).wall = Map::Walls::nothing; break;
      default: break;
      }
    }
  }
}

void RawMap::process_start_position() {
  for (auto h = std::size_t{0u}; h < height(); h++) {
    for (auto w = std::size_t{0u}; w < width(); w++) {
      const auto object = block(w, h).object;
      switch (block(w, h).object) {
      case Map::Objects::start_position_n:
      case Map::Objects::start_position_s:
      case Map::Objects::start_position_w:
      case Map::Objects::start_position_e:
        if (player_pos_w_ != std::numeric_limits<std::size_t>::max() ||
            player_pos_h_ != std::numeric_limits<std::size_t>::max()) {
          throw std::runtime_error{"map error: more than one player position detected"};
        }
        player_pos_w_ = w;
        player_pos_h_ = h;
        break;
      default: break;
      }
    }
  }
  if (player_pos_w_ == std::numeric_limits<std::size_t>::max() ||
      player_pos_h_ == std::numeric_limits<std::size_t>::max()) {
    throw std::runtime_error{"map error: no player position detected"};
  }
}
} // namespace wolf
