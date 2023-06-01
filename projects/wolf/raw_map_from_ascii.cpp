#include "raw_map_from_ascii.hpp"

#include "raw_map.hpp"
#include "wolf_map_info.hpp"

#include <stdexcept>

namespace wolf {
RawMapFromAscii::RawMapFromAscii(const std::string &asciimap_filename) : asciimap_file_{asciimap_filename} {
  if (!asciimap_file_.is_open()) { throw std::runtime_error{"cannot open given map file"}; }
}

std::unique_ptr<RawMap> RawMapFromAscii::create_map() {
  std::size_t width{};
  std::size_t height{};

  asciimap_file_.seekg(0);
  asciimap_file_ >> width;
  asciimap_file_ >> height;
  asciimap_file_.ignore();

  // greater by 1 because getline adds \0 at the end of each read
  auto raw_blocks = std::vector<char>(width * height + 1);
  for (std::size_t h{0u}; h < height; h++) {
    asciimap_file_.getline(raw_blocks.data() + h * width, raw_blocks.size() - h * width);
  }

  auto blocks = std::vector<RawMap::BlockType>(width * height);
  for (std::size_t b_it{0u}; b_it < blocks.size(); b_it++) {
    blocks[b_it].wall   = wolf::Map::Walls::nothing;
    blocks[b_it].object = wolf::Map::Objects::nothing;
    blocks[b_it].extra  = wolf::Map::Extra::nothing;

    switch (raw_blocks[b_it]) {
    case '1': blocks[b_it].wall = wolf::Map::Walls::blue_wall; break;
    case '2': blocks[b_it].wall = wolf::Map::Walls::grey_wall_1; break;
    case '3': blocks[b_it].wall = wolf::Map::Walls::wood; break;
    case '4': blocks[b_it].wall = wolf::Map::Walls::steel; break;
    case '5': blocks[b_it].wall = wolf::Map::Walls::red_brick; break;
    case '6': blocks[b_it].wall = wolf::Map::Walls::multicolor_brick; break;
    case '7': blocks[b_it].wall = wolf::Map::Walls::purple; break;
    case '8': blocks[b_it].wall = wolf::Map::Walls::brown_stone_1; break;
    case '9': blocks[b_it].wall = wolf::Map::Walls::landscape; break;
    case 'n': blocks[b_it].object = wolf::Map::Objects::start_position_n; break;
    case 's': blocks[b_it].object = wolf::Map::Objects::start_position_s; break;
    case 'w': blocks[b_it].object = wolf::Map::Objects::start_position_w; break;
    case 'e': blocks[b_it].object = wolf::Map::Objects::start_position_e; break;
    default: break;
    }
  }

  return std::make_unique<RawMap>(width, height, std::move(blocks));
}
} // namespace wolf
