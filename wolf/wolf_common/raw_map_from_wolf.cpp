#include "raw_map_from_wolf.hpp"

#include <array>
#include <stdexcept>

namespace wolf {
RawMapFromWolf::RawMapFromWolf(const std::string &maphead_filename, const std::string &gamemaps_filename)
    : gamemaps_file_{gamemaps_filename, std::ios::binary | std::ios::in} {
  auto maphead_file = std::ifstream{maphead_filename};

  if (!maphead_file.is_open() || !gamemaps_file_.is_open()) {
    throw std::runtime_error{"cannot open given map file(s)"};
  }

#pragma pack(push, 1)

  struct {
    std::uint16_t RLEWtag;
    std::int32_t headeroffsets[100];
    /* skipped:
    std::uint8_t  tileinfo[];
    */
  } map_file_type;

#pragma pack(pop)

  maphead_file.read(reinterpret_cast<char *>(&map_file_type), sizeof(map_file_type));

  rlew_tag_ = map_file_type.RLEWtag;
  for (const auto &v : map_file_type.headeroffsets) {
    if (v > 0) {
      header_offsets_.push_back(v);
    }
  }
}

std::size_t RawMapFromWolf::number_of_maps() const { return header_offsets_.size(); }

std::unique_ptr<const RawMap> RawMapFromWolf::create_map(const std::size_t map_index) {
#pragma pack(push, 1)

  struct {
    std::int32_t planestart[3];
    std::uint16_t planelength[3];
    std::uint16_t width, height;
    char name[16];
  } map_type;

#pragma pack(pop)

  gamemaps_file_.seekg(header_offsets_.at(map_index));
  gamemaps_file_.read(reinterpret_cast<char *>(&map_type), sizeof(map_type));

  auto compressed_planes = std::array<std::vector<std::uint16_t>, 3>{};
  for (auto c_it = std::size_t{0u}; c_it < compressed_planes.size(); c_it++) {
    compressed_planes[c_it].resize(map_type.planelength[c_it]);
    gamemaps_file_.seekg(map_type.planestart[c_it]);
    gamemaps_file_.read(reinterpret_cast<char *>(compressed_planes[c_it].data()),
                        sizeof(compressed_planes[c_it][0]) * compressed_planes[c_it].size());
  }

  auto blocks = std::vector<RawMap::BlockType>(map_type.width * map_type.height);
  decarmacize_plane(compressed_planes[0], blocks, 0u);
  decarmacize_plane(compressed_planes[1], blocks, 1u);
  decarmacize_plane(compressed_planes[2], blocks, 2u);

  return std::make_unique<RawMap>(map_type.width, map_type.height, std::move(blocks));
}

void RawMapFromWolf::decarmacize_plane(const std::vector<std::uint16_t> &plane_data,
                                       std::vector<RawMap::BlockType> &blocks,
                                       const std::size_t plane) const {
  constexpr auto near_tag = std::uint16_t{0xa7};
  constexpr auto far_tag = std::uint16_t{0xa8};

  auto extended_plane = std::vector<std::uint16_t>(plane_data.at(0u) / 2u);
  auto in_ptr = reinterpret_cast<const std::uint8_t *>(&plane_data.at(1u));

  for (auto e_it = std::size_t{0u}; e_it < extended_plane.size(); e_it++) {
    std::uint16_t ch;
    std::memcpy(&ch, in_ptr, 2);
    in_ptr += 2;

    if ((ch >> 8u) == near_tag) {
      const auto count = ch & 0xff;

      if (count == 0u) {
        extended_plane[e_it] = ch | *in_ptr++;
      } else {
        const auto offset = std::uint16_t{*in_ptr++};
        for (auto c = std::size_t{0u}; c < count; c++) {
          extended_plane[e_it + c] = extended_plane[(e_it - offset) + c];
        }
        e_it += count - 1u;
      }
    } else if ((ch >> 8u) == far_tag) {
      const auto count = ch & 0xff;

      if (count == 0u) {
        extended_plane[e_it] = ch | *in_ptr++;
      } else {
        std::uint16_t offset;
        memcpy(&offset, in_ptr, 2);
        in_ptr += 2;
        for (auto c = std::size_t{0u}; c < count; c++) {
          extended_plane[e_it + c] = extended_plane[offset + c];
        }
        e_it += count - 1u;
      }
    } else {
      extended_plane[e_it] = ch;
    }
  }

  expand_plane(extended_plane, blocks, plane);
}

void RawMapFromWolf::expand_plane(const std::vector<std::uint16_t> &plane_data,
                                  std::vector<RawMap::BlockType> &blocks,
                                  const std::size_t plane) const {
  for (auto b_it = std::size_t{0u}, p_it = std::size_t{1u}; b_it < blocks.size(); b_it++) {
    blocks[b_it][plane] = plane_data.at(p_it++);

    if (blocks[b_it][plane] != rlew_tag_) {
      continue;
    }

    const auto count = plane_data.at(p_it++);
    const auto value = plane_data.at(p_it++);
    if (count != 0) {
      for (auto c = std::size_t{0u}; c < count; c++) {
        blocks[b_it + c][plane] = value;
      }
    }
    b_it += count - 1u;
  }
}
} // namespace wolf
