#include "vswap_file.hpp"

#include <algorithm>
#include <fstream>
#include <stdexcept>

namespace wolf {
namespace {
// clang-format off
constexpr auto COLOR_PALETTE = std::array<glm::u8vec4, 256>{{
  {  0,   0,   0, 255}, {  0,   0, 168, 255}, {  0, 168,   0, 255}, {  0, 168, 168, 255},
  {168,   0,   0, 255}, {168,   0, 168, 255}, {168,  84,   0, 255}, {168, 168, 168, 255},
  { 84,  84,  84, 255}, { 84,  84, 252, 255}, { 84, 252,  84, 255}, { 84, 252, 252, 255},
  {252,  84,  84, 255}, {252,  84, 252, 255}, {252, 252,  84, 255}, {252, 252, 252, 255},
  {236, 236, 236, 255}, {220, 220, 220, 255}, {208, 208, 208, 255}, {192, 192, 192, 255},
  {180, 180, 180, 255}, {168, 168, 168, 255}, {152, 152, 152, 255}, {140, 140, 140, 255},
  {124, 124, 124, 255}, {112, 112, 112, 255}, {100, 100, 100, 255}, { 84,  84,  84, 255},
  { 72,  72,  72, 255}, { 56,  56,  56, 255}, { 44,  44,  44, 255}, { 32,  32,  32, 255},
  {252,   0,   0, 255}, {236,   0,   0, 255}, {224,   0,   0, 255}, {212,   0,   0, 255},
  {200,   0,   0, 255}, {188,   0,   0, 255}, {176,   0,   0, 255}, {164,   0,   0, 255},
  {152,   0,   0, 255}, {136,   0,   0, 255}, {124,   0,   0, 255}, {112,   0,   0, 255},
  {100,   0,   0, 255}, { 88,   0,   0, 255}, { 76,   0,   0, 255}, { 64,   0,   0, 255},
  {252, 216, 216, 255}, {252, 184, 184, 255}, {252, 156, 156, 255}, {252, 124, 124, 255},
  {252,  92,  92, 255}, {252,  64,  64, 255}, {252,  32,  32, 255}, {252,   0,   0, 255},
  {252, 168,  92, 255}, {252, 152,  64, 255}, {252, 136,  32, 255}, {252, 120,   0, 255},
  {228, 108,   0, 255}, {204,  96,   0, 255}, {180,  84,   0, 255}, {156,  76,   0, 255},
  {252, 252, 216, 255}, {252, 252, 184, 255}, {252, 252, 156, 255}, {252, 252, 124, 255},
  {252, 248,  92, 255}, {252, 244,  64, 255}, {252, 244,  32, 255}, {252, 244,   0, 255},
  {228, 216,   0, 255}, {204, 192,   0, 255}, {180, 172,   0, 255}, {156, 156,   0, 255},
  {132, 132,   0, 255}, {112, 108,   0, 255}, { 88,  84,   0, 255}, { 64,  64,   0, 255},
  {208, 252,  92, 255}, {192, 252,  64, 255}, {180, 252,  32, 255}, {160, 252,   0, 255},
  {144, 228,   0, 255}, {128, 204,   0, 255}, {116, 180,   0, 255}, { 96, 156,   0, 255},
  {216, 252, 216, 255}, {188, 252, 184, 255}, {156, 252, 156, 255}, {128, 252, 124, 255},
  { 96, 252,  92, 255}, { 64, 252,  64, 255}, { 32, 252,  32, 255}, {  0, 252,   0, 255},
  {  0, 252,   0, 255}, {  0, 236,   0, 255}, {  0, 224,   0, 255}, {  0, 212,   0, 255},
  {  0, 200,   0, 255}, {  0, 188,   0, 255}, {  0, 176,   0, 255}, {  0, 164,   0, 255},
  {  0, 152,   0, 255}, {  0, 136,   0, 255}, {  0, 124,   0, 255}, {  0, 112,   0, 255},
  {  0, 100,   0, 255}, {  0,  88,   0, 255}, {  0,  76,   0, 255}, {  0,  64,   0, 255},
  {216, 252, 252, 255}, {184, 252, 252, 255}, {156, 252, 252, 255}, {124, 252, 248, 255},
  { 92, 252, 252, 255}, { 64, 252, 252, 255}, { 32, 252, 252, 255}, {  0, 252, 252, 255},
  {  0, 228, 228, 255}, {  0, 204, 204, 255}, {  0, 180, 180, 255}, {  0, 156, 156, 255},
  {  0, 132, 132, 255}, {  0, 112, 112, 255}, {  0,  88,  88, 255}, {  0,  64,  64, 255},
  { 92, 188, 252, 255}, { 64, 176, 252, 255}, { 32, 168, 252, 255}, {  0, 156, 252, 255},
  {  0, 140, 228, 255}, {  0, 124, 204, 255}, {  0, 108, 180, 255}, {  0,  92, 156, 255},
  {216, 216, 252, 255}, {184, 188, 252, 255}, {156, 156, 252, 255}, {124, 128, 252, 255},
  { 92,  96, 252, 255}, { 64,  64, 252, 255}, { 32,  36, 252, 255}, {  0,   4, 252, 255},
  {  0,   0, 252, 255}, {  0,   0, 236, 255}, {  0,   0, 224, 255}, {  0,   0, 212, 255},
  {  0,   0, 200, 255}, {  0,   0, 188, 255}, {  0,   0, 176, 255}, {  0,   0, 164, 255},
  {  0,   0, 152, 255}, {  0,   0, 136, 255}, {  0,   0, 124, 255}, {  0,   0, 112, 255},
  {  0,   0, 100, 255}, {  0,   0,  88, 255}, {  0,   0,  76, 255}, {  0,   0,  64, 255},
  { 40,  40,  40, 255}, {252, 224,  52, 255}, {252, 212,  36, 255}, {252, 204,  24, 255},
  {252, 192,   8, 255}, {252, 180,   0, 255}, {180,  32, 252, 255}, {168,   0, 252, 255},
  {152,   0, 228, 255}, {128,   0, 204, 255}, {116,   0, 180, 255}, { 96,   0, 156, 255},
  { 80,   0, 132, 255}, { 68,   0, 112, 255}, { 52,   0,  88, 255}, { 40,   0,  64, 255},
  {252, 216, 252, 255}, {252, 184, 252, 255}, {252, 156, 252, 255}, {252, 124, 252, 255},
  {252,  92, 252, 255}, {252,  64, 252, 255}, {252,  32, 252, 255}, {252,   0, 252, 255},
  {224,   0, 228, 255}, {200,   0, 204, 255}, {180,   0, 180, 255}, {156,   0, 156, 255},
  {132,   0, 132, 255}, {108,   0, 112, 255}, { 88,   0,  88, 255}, { 64,   0,  64, 255},
  {252, 232, 220, 255}, {252, 224, 208, 255}, {252, 216, 196, 255}, {252, 212, 188, 255},
  {252, 204, 176, 255}, {252, 196, 164, 255}, {252, 188, 156, 255}, {252, 184, 144, 255},
  {252, 176, 128, 255}, {252, 164, 112, 255}, {252, 156,  96, 255}, {240, 148,  92, 255},
  {232, 140,  88, 255}, {220, 136,  84, 255}, {208, 128,  80, 255}, {200, 124,  76, 255},
  {188, 120,  72, 255}, {180, 112,  68, 255}, {168, 104,  64, 255}, {160, 100,  60, 255},
  {156,  96,  56, 255}, {144,  92,  52, 255}, {136,  88,  48, 255}, {128,  80,  44, 255},
  {116,  76,  40, 255}, {108,  72,  36, 255}, { 92,  64,  32, 255}, { 84,  60,  28, 255},
  { 72,  56,  24, 255}, { 64,  48,  24, 255}, { 56,  44,  20, 255}, { 40,  32,  12, 255},
  { 96,   0, 100, 255}, {  0, 100, 100, 255}, {  0,  96,  96, 255}, {  0,   0,  28, 255},
  {  0,   0,  44, 255}, { 48,  36,  16, 255}, { 72,   0,  72, 255}, { 80,   0,  80, 255},
  {  0,   0,  52, 255}, { 28,  28,  28, 255}, { 76,  76,  76, 255}, { 92,  92,  92, 255},
  { 64,  64,  64, 255}, { 48,  48,  48, 255}, { 52,  52,  52, 255}, {216, 244, 244, 255},
  {184, 232, 232, 255}, {156, 220, 220, 255}, {116, 200, 200, 255}, { 72, 192, 192, 255},
  { 32, 180, 180, 255}, { 32, 176, 176, 255}, {  0, 164, 164, 255}, {  0, 152, 152, 255},
  {  0, 140, 140, 255}, {  0, 132, 132, 255}, {  0, 124, 124, 255}, {  0, 120, 120, 255},
  {  0, 116, 116, 255}, {  0, 112, 112, 255}, {  0, 108, 108, 255}, {152,   0, 136, 255}
}};
// clang-format on
} // namespace

VswapFile::VswapFile(const std::string &vswap_filename) {
  auto vswap_file = std::ifstream(vswap_filename, std::ios::binary | std::ios::in);
  if (!vswap_file) {
    throw std::runtime_error("Could not open file " + vswap_filename);
  }

  std::uint16_t number_of_all_chunks{};
  vswap_file.read(reinterpret_cast<char *>(&number_of_all_chunks), sizeof(number_of_all_chunks));
  std::uint16_t index_of_first_sprite_chunk{};
  vswap_file.read(reinterpret_cast<char *>(&index_of_first_sprite_chunk), sizeof(index_of_first_sprite_chunk));
  std::uint16_t index_of_first_sound_chunk{};
  vswap_file.read(reinterpret_cast<char *>(&index_of_first_sound_chunk), sizeof(index_of_first_sound_chunk));

  const auto number_of_walls = index_of_first_sprite_chunk;
  const auto number_of_sprites = index_of_first_sound_chunk - index_of_first_sprite_chunk;
  const auto number_of_sounds = number_of_all_chunks - index_of_first_sound_chunk;

  walls_.resize(number_of_walls);
  sprites_.resize(number_of_sprites);
  sprites_opaque_.resize(number_of_sprites);
  (void)number_of_sounds; // TODO: implement sound chunks

  std::vector<std::uint32_t> chunk_offsets(number_of_all_chunks);
  vswap_file.read(reinterpret_cast<char *>(chunk_offsets.data()), sizeof(chunk_offsets[0]) * chunk_offsets.size());

  std::vector<std::uint16_t> chunk_lengths(number_of_all_chunks);
  vswap_file.read(reinterpret_cast<char *>(chunk_lengths.data()), sizeof(chunk_lengths[0]) * chunk_lengths.size());

  decode_walls(vswap_file, chunk_offsets);
  decode_sprites(vswap_file, chunk_offsets, chunk_lengths);
}

const std::vector<std::array<glm::u8vec4, 4096>> &VswapFile::walls() const noexcept { return walls_; }

const std::vector<std::array<glm::u8vec4, 4096>> &VswapFile::sprites() const noexcept { return sprites_; }

const std::vector<std::array<bool, 4096>> &VswapFile::sprites_opaque() const noexcept { return sprites_opaque_; }

void VswapFile::decode_walls(std::ifstream &vswap_file, const std::vector<std::uint32_t> &chunk_offsets) {
  constexpr auto wall_width = 64;
  constexpr auto wall_size = wall_width * wall_width;

  std::vector<std::uint8_t> color_codes(wall_size);

  for (std::uint16_t i = 0; i < walls_.size(); ++i) {
    vswap_file.seekg(chunk_offsets[i]);
    vswap_file.read(reinterpret_cast<char *>(color_codes.data()), wall_size);

    for (std::size_t x = 0; x < wall_width; ++x) {
      for (std::size_t y = 0; y < wall_width; ++y) {
        const auto src_pos = x * wall_width + y;
        const auto dst_pos = y * wall_width + x;
        walls_[i][dst_pos] = COLOR_PALETTE[color_codes[src_pos]];
      }
    }
  }
}

void VswapFile::decode_sprites(std::ifstream &vswap_file,
                               const std::vector<std::uint32_t> &chunk_offsets,
                               const std::vector<std::uint16_t> &chunk_lengths) {
  constexpr auto sprite_width = 64;
  constexpr auto sprite_size = sprite_width * sprite_width;

  std::vector<std::int16_t> color_codes(sprite_size);
  std::vector<std::uint16_t> offsets;
  std::vector<std::uint8_t> pixel_pool;
  std::vector<std::uint16_t> piece_info;

  for (std::uint16_t i = 0; i < sprites_.size(); ++i) {
    const auto chunk_offset = chunk_offsets[i + walls_.size()];
    vswap_file.seekg(chunk_offset);

    std::uint16_t first_col{};
    vswap_file.read(reinterpret_cast<char *>(&first_col), sizeof(first_col));
    std::uint16_t last_col{};
    vswap_file.read(reinterpret_cast<char *>(&last_col), sizeof(last_col));

    std::fill(color_codes.begin(), color_codes.end(), -1);
    auto color_codes_it = color_codes.begin() + first_col * sprite_width;

    const auto number_of_columns = last_col - first_col + 1;
    offsets.resize(number_of_columns + 1);
    vswap_file.read(reinterpret_cast<char *>(offsets.data()), sizeof(offsets[0]) * number_of_columns);
    offsets.back() = chunk_lengths[i + walls_.size()];

    const auto pixel_pool_size =
        offsets[0] - (sizeof(first_col) + sizeof(last_col) + sizeof(offsets[0]) * number_of_columns);
    pixel_pool.resize(pixel_pool_size);
    vswap_file.read(reinterpret_cast<char *>(pixel_pool.data()), pixel_pool.size());
    auto pixel_pool_it = pixel_pool.begin();

    for (std::uint16_t j = 0; j < number_of_columns; ++j) {
      const auto piece_size = (offsets[j + 1] - offsets[j]) / 2;
      piece_info.resize(piece_size);
      vswap_file.read(reinterpret_cast<char *>(piece_info.data()), sizeof(piece_info[0]) * piece_size);
      const auto pieces_number = piece_size / 3;
      for (int k = 0; k < pieces_number; ++k) {
        const auto piece_end = piece_info[k * 3] / 2;
        const auto piece_start = piece_info[k * 3 + 2] / 2;
        const auto color_codes_write_it = color_codes_it + piece_start;
        const auto copy_size = piece_end - piece_start;
        std::copy_n(pixel_pool_it, copy_size, color_codes_write_it);
        pixel_pool_it += copy_size;
      }
      color_codes_it += sprite_width;
    }

    for (std::size_t x = 0; x < sprite_width; ++x) {
      for (std::size_t y = 0; y < sprite_width; ++y) {
        const auto src_pos = x * sprite_width + y;
        const auto dst_pos = y * sprite_width + x;
        if (color_codes[src_pos] == -1) {
          sprites_[i][dst_pos] = {0.0f, 0.0f, 0.0f, 0.0f};
          sprites_opaque_[i][dst_pos] = false;
        } else {
          sprites_[i][dst_pos] = COLOR_PALETTE[color_codes[src_pos]];
          sprites_opaque_[i][dst_pos] = true;
        }
      }
    }
  }
}
} // namespace wolf
