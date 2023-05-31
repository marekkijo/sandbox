#include "raw_map_from_wolf.hpp"

#include "raw_map.hpp"

#include <stdexcept>

namespace wolf {
RawMapFromWolf::RawMapFromWolf(const std::string &maphead_filename, const std::string &gamemaps_filename)
    : maphead_file_{maphead_filename}, gamemaps_file_{gamemaps_filename} {
  if (!maphead_file_.is_open() || !gamemaps_file_.is_open()) {
    throw std::runtime_error{"cannot open given map file(s)"};
  }
}

std::unique_ptr<RawMap> RawMapFromWolf::create_map(const std::size_t map_index) {
  std::size_t                    width{1};
  std::size_t                    height{1};
  std::vector<RawMap::BlockType> blocks(1, 'e');

  return std::make_unique<RawMap>(width, height, std::move(blocks));
}
} // namespace wolf
