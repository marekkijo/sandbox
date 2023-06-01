#include "raw_map_from_ascii.hpp"

#include "raw_map.hpp"

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
    if (raw_blocks[b_it] >= '0' && raw_blocks[b_it] <= '9') {
      blocks[b_it][0] = raw_blocks[b_it] - '0';
    } else if (raw_blocks[b_it] == 'n' || raw_blocks[b_it] == 's' || raw_blocks[b_it] == 'w' ||
               raw_blocks[b_it] == 'e') {
      blocks[b_it][0] = raw_blocks[b_it];
    } else {
      blocks[b_it][0] = 0u;
    }

    blocks[b_it][1] = 0u;
    blocks[b_it][2] = 0u;
  }

  return std::make_unique<RawMap>(width, height, std::move(blocks));
}
} // namespace wolf
