#include "raw_map.hpp"

#include <fstream>
#include <stdexcept>

namespace wolf {
RawMap::RawMap(const std::string &filename) {
  auto file = std::ifstream{filename};
  if (!file.is_open()) {
    throw std::runtime_error{"cannot open given map file"};
  }

  file >> width_;
  file >> height_;
  blocks_.resize(width() * height() + 1);

  file.ignore();
  for (std::size_t h{0u}; h < height(); h++) {
    file.getline(blocks_.data() + h * width(), blocks_.size() - h * width());
    if (file.gcount() != width() + 1) {
      throw std::runtime_error{"error while reading given map file"};
    }
  }

  detect_player_pos();
}

std::size_t RawMap::width() const { return width_; }

std::size_t RawMap::height() const { return height_; }

const RawMap::BlockType &RawMap::block(std::size_t w, std::size_t h) const { return blocks_[w + h * width()]; }

bool RawMap::is_wall(std::size_t w, std::size_t h) const {
  const auto &_block = block(w, h);
  return _block >= '1' && _block <= '9';
}

bool RawMap::is_wall_on_n(std::size_t w, std::size_t h) const { return h == 0 || is_wall(w, h - 1); }

bool RawMap::is_wall_on_s(std::size_t w, std::size_t h) const { return h == (height() - 1) || is_wall(w, h + 1); }

bool RawMap::is_wall_on_w(std::size_t w, std::size_t h) const { return w == 0 || is_wall(w - 1, h); }

bool RawMap::is_wall_on_e(std::size_t w, std::size_t h) const { return w == (width() - 1) || is_wall(w + 1, h); }

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
