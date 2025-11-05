#pragma once

#include "wolf_common/raw_map.hpp"

#include <cstdint>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

namespace wolf {
class RawMapFromWolf {
public:
  RawMapFromWolf(const std::string &maphead_filename, const std::string &gamemaps_filename);

  std::size_t number_of_maps() const;
  std::unique_ptr<const RawMap> create_map(const std::size_t map_index);

private:
  std::ifstream gamemaps_file_{};
  std::uint16_t rlew_tag_{};
  std::vector<std::size_t> header_offsets_{};

  void decarmacize_plane(const std::vector<std::uint16_t> &plane_data,
                         std::vector<RawMap::BlockType> &blocks,
                         const std::size_t plane = 0u) const;
  void expand_plane(const std::vector<std::uint16_t> &plane_data,
                    std::vector<RawMap::BlockType> &blocks,
                    const std::size_t plane = 0u) const;
};
} // namespace wolf
