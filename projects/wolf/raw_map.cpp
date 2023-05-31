#include "raw_map.hpp"

#include <stdexcept>

namespace wolf {
RawMap::RawMap(const std::size_t width, const std::size_t height, const std::vector<BlockType> &blocks)
    : width_{width}, height_{height}, blocks_{blocks} {
  detect_player_pos();
}

RawMap::RawMap(const std::size_t width, const std::size_t height, std::vector<BlockType> &&blocks)
    : width_{width}, height_{height}, blocks_{std::move(blocks)} {
  detect_player_pos();
}

std::size_t RawMap::width() const { return width_; }

std::size_t RawMap::height() const { return height_; }

const RawMap::BlockType &RawMap::block(const std::size_t w, const std::size_t h) const {
  return blocks_[w + h * width()];
}

bool RawMap::is_wall(const std::size_t w, const std::size_t h) const {
  const auto &_block = block(w, h);
  return _block >= '1' && _block <= '9';
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

void RawMap::detect_player_pos() {
  for (std::size_t h{0u}; h < height(); h++) {
    for (std::size_t w{0u}; w < width(); w++) {
      const auto &_block = block(w, h);
      if (_block == 'n' || _block == 's' || _block == 'w' || _block == 'e') {
        if (player_pos_w_ != std::numeric_limits<std::size_t>::max() ||
            player_pos_h_ != std::numeric_limits<std::size_t>::max()) {
          throw std::runtime_error{"map error: more than one player position detected"};
        }
        player_pos_w_ = w;
        player_pos_h_ = h;
      }
    }
  }
  if (player_pos_w_ == std::numeric_limits<std::size_t>::max() ||
      player_pos_h_ == std::numeric_limits<std::size_t>::max()) {
    throw std::runtime_error{"map error: no player position detected"};
  }
}
} // namespace wolf
