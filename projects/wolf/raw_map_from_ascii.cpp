#include "raw_map_from_ascii.hpp"

#include "raw_map.hpp"

#include <stdexcept>

namespace wolf {
RawMapFromAscii::RawMapFromAscii(const std::string &asciimap_filename) : asciimap_file_{asciimap_filename} {
  if (!asciimap_file_.is_open()) { throw std::runtime_error{"cannot open given map file"}; }
}

std::unique_ptr<RawMap> RawMapFromAscii::create_map() {
  std::size_t                    width{};
  std::size_t                    height{};
  std::vector<RawMap::BlockType> blocks{};

  asciimap_file_ >> width;
  asciimap_file_ >> height;
  blocks.resize(width * height + 1);

  asciimap_file_.ignore();
  for (std::size_t h{0u}; h < height; h++) {
    asciimap_file_.getline(blocks.data() + h * width, blocks.size() - h * width);
    if (asciimap_file_.gcount() != width + 1) { throw std::runtime_error{"error while reading given map file"}; }
  }
  asciimap_file_.seekg(0);

  return std::make_unique<RawMap>(width, height, std::move(blocks));
}
} // namespace wolf
