#include "sprite_voxelizer.hpp"

#include <algorithm>
#include <iterator>
#include <stdexcept>

namespace wolf {
namespace {
constexpr auto SPRITE_WIDTH = 64;

template<typename... Ts>
constexpr auto make_array(Ts &&...args) {
  return std::array<std::common_type_t<Ts...>, sizeof...(Ts)>{std::forward<Ts>(args)...};
}
} // namespace

SpriteVoxelizer::SpriteVoxelizer(std::shared_ptr<const VswapFile> vswap_file)
    : vswap_file_{std::move(vswap_file)} {}

void SpriteVoxelizer::voxelize(const std::size_t front_sprite_index,
                               const std::size_t back_sprite_index,
                               const std::size_t left_sprite_index,
                               const std::size_t right_sprite_index) {
  if (front_sprite_index >= vswap_file_->sprites().size() || back_sprite_index >= vswap_file_->sprites().size() ||
      left_sprite_index >= vswap_file_->sprites().size() || right_sprite_index >= vswap_file_->sprites().size()) {
    throw std::runtime_error("Invalid sprite index");
  }
  const auto voxel_map =
      prepare_voxel_map(front_sprite_index, back_sprite_index, left_sprite_index, right_sprite_index);
  build_voxel_model(voxel_map);
}

void SpriteVoxelizer::voxelize(const std::size_t sprite_index, const bool mirrored_sides) {
  if (sprite_index >= vswap_file_->sprites().size()) {
    throw std::runtime_error("Invalid sprite index");
  }
  const auto voxel_map = prepare_voxel_map(sprite_index, mirrored_sides);
  build_voxel_model(voxel_map);
}

const std::vector<SpriteVoxelizer::VoxelVertexData> &SpriteVoxelizer::vertex_data() const { return vertex_data_; }

const std::vector<GLuint> &SpriteVoxelizer::indices() const { return indices_; }

std::vector<SpriteVoxelizer::VoxelPoint> SpriteVoxelizer::prepare_voxel_map(const std::size_t front_sprite_index,
                                                                            const std::size_t back_sprite_index,
                                                                            const std::size_t left_sprite_index,
                                                                            const std::size_t right_sprite_index) {
  auto voxel_map = std::vector<VoxelPoint>(SPRITE_WIDTH * SPRITE_WIDTH * SPRITE_WIDTH);
  auto voxel_opaque = std::vector<bool>(SPRITE_WIDTH * SPRITE_WIDTH * SPRITE_WIDTH, false);

  const auto &front_sprite = vswap_file_->sprites()[front_sprite_index];
  const auto &front_opaque = vswap_file_->sprites_opaque()[front_sprite_index];
  const auto &back_sprite = vswap_file_->sprites()[back_sprite_index];
  const auto &back_opaque = vswap_file_->sprites_opaque()[back_sprite_index];
  for (int x = 0; x < SPRITE_WIDTH; ++x) {
    for (int y = 0; y < SPRITE_WIDTH; ++y) {
      const auto sample_x = SPRITE_WIDTH - x - 1;
      if (front_opaque[y * SPRITE_WIDTH + sample_x]) {
        for (int z = 0; z < SPRITE_WIDTH / 2; ++z) {
          voxel_map[z * SPRITE_WIDTH * SPRITE_WIDTH + y * SPRITE_WIDTH + x].color =
              front_sprite[y * SPRITE_WIDTH + sample_x];
          voxel_opaque[z * SPRITE_WIDTH * SPRITE_WIDTH + y * SPRITE_WIDTH + x] = true;
        }
      }

      if (back_opaque[y * SPRITE_WIDTH + x]) {
        for (int z = SPRITE_WIDTH / 2; z < SPRITE_WIDTH; ++z) {
          voxel_map[z * SPRITE_WIDTH * SPRITE_WIDTH + y * SPRITE_WIDTH + x].color = back_sprite[y * SPRITE_WIDTH + x];
          voxel_opaque[z * SPRITE_WIDTH * SPRITE_WIDTH + y * SPRITE_WIDTH + x] = true;
        }
      }
    }
  }

  const auto &left_opaque = vswap_file_->sprites_opaque()[left_sprite_index];
  const auto &right_opaque = vswap_file_->sprites_opaque()[right_sprite_index];
  for (int x = 0; x < SPRITE_WIDTH; ++x) {
    for (int y = 0; y < SPRITE_WIDTH; ++y) {
      if (left_opaque[y * SPRITE_WIDTH + x]) {
        for (int z = 0; z < SPRITE_WIDTH / 2; ++z) {
          voxel_map[x * SPRITE_WIDTH * SPRITE_WIDTH + y * SPRITE_WIDTH + z].opaque =
              voxel_opaque[x * SPRITE_WIDTH * SPRITE_WIDTH + y * SPRITE_WIDTH + z];
        }
      }

      const auto sample_x = SPRITE_WIDTH - x - 1;
      if (right_opaque[y * SPRITE_WIDTH + sample_x]) {
        for (int z = SPRITE_WIDTH / 2; z < SPRITE_WIDTH; ++z) {
          voxel_map[x * SPRITE_WIDTH * SPRITE_WIDTH + y * SPRITE_WIDTH + z].opaque =
              voxel_opaque[x * SPRITE_WIDTH * SPRITE_WIDTH + y * SPRITE_WIDTH + z];
        }
      }
    }
  }

  const auto &left_sprite = vswap_file_->sprites()[left_sprite_index];
  const auto &right_sprite = vswap_file_->sprites()[right_sprite_index];
  for (int x = 1; x < SPRITE_WIDTH - 1; ++x) {
    for (int y = 0; y < SPRITE_WIDTH; ++y) {
      if (left_opaque[y * SPRITE_WIDTH + x]) {
        for (int z = 0; z < SPRITE_WIDTH / 2; ++z) {
          if (voxel_map[(x - 1) * SPRITE_WIDTH * SPRITE_WIDTH + y * SPRITE_WIDTH + z].opaque &&
              voxel_map[(x + 1) * SPRITE_WIDTH * SPRITE_WIDTH + y * SPRITE_WIDTH + z].opaque) {
            voxel_map[x * SPRITE_WIDTH * SPRITE_WIDTH + y * SPRITE_WIDTH + z].color = left_sprite[y * SPRITE_WIDTH + x];
          }
        }
      }

      const auto sample_x = SPRITE_WIDTH - x - 1;
      if (right_opaque[y * SPRITE_WIDTH + sample_x]) {
        for (int z = SPRITE_WIDTH / 2; z < SPRITE_WIDTH; ++z) {
          if (voxel_map[(x - 1) * SPRITE_WIDTH * SPRITE_WIDTH + y * SPRITE_WIDTH + z].opaque &&
              voxel_map[(x + 1) * SPRITE_WIDTH * SPRITE_WIDTH + y * SPRITE_WIDTH + z].opaque) {
            voxel_map[x * SPRITE_WIDTH * SPRITE_WIDTH + y * SPRITE_WIDTH + z].color =
                right_sprite[y * SPRITE_WIDTH + sample_x];
          }
        }
      }
    }
  }

  return voxel_map;
}

std::vector<SpriteVoxelizer::VoxelPoint> SpriteVoxelizer::prepare_voxel_map(const std::size_t sprite_index,
                                                                            const bool mirrored_sides) {
  auto voxel_map = std::vector<VoxelPoint>(SPRITE_WIDTH * SPRITE_WIDTH * SPRITE_WIDTH);
  auto voxel_opaque = std::vector<bool>(SPRITE_WIDTH * SPRITE_WIDTH * SPRITE_WIDTH, false);

  const auto &sprite = vswap_file_->sprites()[sprite_index];
  const auto &opaque = vswap_file_->sprites_opaque()[sprite_index];
  for (int x = 0; x < SPRITE_WIDTH; ++x) {
    for (int y = 0; y < SPRITE_WIDTH; ++y) {
      const auto sample_x = SPRITE_WIDTH - x - 1;
      if (opaque[y * SPRITE_WIDTH + sample_x]) {
        for (int z = 0; z < SPRITE_WIDTH / 2; ++z) {
          voxel_map[z * SPRITE_WIDTH * SPRITE_WIDTH + y * SPRITE_WIDTH + x].color = sprite[y * SPRITE_WIDTH + sample_x];
          voxel_opaque[z * SPRITE_WIDTH * SPRITE_WIDTH + y * SPRITE_WIDTH + x] = true;
        }
      }

      if (opaque[y * SPRITE_WIDTH + x]) {
        for (int z = SPRITE_WIDTH / 2; z < SPRITE_WIDTH; ++z) {
          voxel_map[z * SPRITE_WIDTH * SPRITE_WIDTH + y * SPRITE_WIDTH + x].color = sprite[y * SPRITE_WIDTH + x];
          voxel_opaque[z * SPRITE_WIDTH * SPRITE_WIDTH + y * SPRITE_WIDTH + x] = true;
        }
      }
    }
  }

  for (int x = 0; x < SPRITE_WIDTH; ++x) {
    for (int y = 0; y < SPRITE_WIDTH; ++y) {
      const auto sample1_x = mirrored_sides ? SPRITE_WIDTH - x - 1 : x;
      const auto sample2_x = mirrored_sides ? x : SPRITE_WIDTH - x - 1;

      if (opaque[y * SPRITE_WIDTH + sample1_x]) {
        for (int z = 0; z < SPRITE_WIDTH / 2; ++z) {
          voxel_map[x * SPRITE_WIDTH * SPRITE_WIDTH + y * SPRITE_WIDTH + z].opaque =
              voxel_opaque[x * SPRITE_WIDTH * SPRITE_WIDTH + y * SPRITE_WIDTH + z];
        }
      }

      if (opaque[y * SPRITE_WIDTH + sample2_x]) {
        for (int z = SPRITE_WIDTH / 2; z < SPRITE_WIDTH; ++z) {
          voxel_map[x * SPRITE_WIDTH * SPRITE_WIDTH + y * SPRITE_WIDTH + z].opaque =
              voxel_opaque[x * SPRITE_WIDTH * SPRITE_WIDTH + y * SPRITE_WIDTH + z];
        }
      }
    }
  }

  for (int x = 1; x < SPRITE_WIDTH - 1; ++x) {
    for (int y = 0; y < SPRITE_WIDTH; ++y) {
      const auto sample1_x = mirrored_sides ? SPRITE_WIDTH - x - 1 : x;
      const auto sample2_x = mirrored_sides ? x : SPRITE_WIDTH - x - 1;

      if (opaque[y * SPRITE_WIDTH + sample1_x]) {
        for (int z = 0; z < SPRITE_WIDTH / 2; ++z) {
          if (voxel_map[(x - 1) * SPRITE_WIDTH * SPRITE_WIDTH + y * SPRITE_WIDTH + z].opaque &&
              voxel_map[(x + 1) * SPRITE_WIDTH * SPRITE_WIDTH + y * SPRITE_WIDTH + z].opaque) {
            voxel_map[x * SPRITE_WIDTH * SPRITE_WIDTH + y * SPRITE_WIDTH + z].color =
                sprite[y * SPRITE_WIDTH + sample1_x];
          }
        }
      }

      if (opaque[y * SPRITE_WIDTH + sample2_x]) {
        for (int z = SPRITE_WIDTH / 2; z < SPRITE_WIDTH; ++z) {
          if (voxel_map[(x - 1) * SPRITE_WIDTH * SPRITE_WIDTH + y * SPRITE_WIDTH + z].opaque &&
              voxel_map[(x + 1) * SPRITE_WIDTH * SPRITE_WIDTH + y * SPRITE_WIDTH + z].opaque) {
            voxel_map[x * SPRITE_WIDTH * SPRITE_WIDTH + y * SPRITE_WIDTH + z].color =
                sprite[y * SPRITE_WIDTH + sample2_x];
          }
        }
      }
    }
  }

  return voxel_map;
}

void SpriteVoxelizer::build_voxel_model(const std::vector<VoxelPoint> &voxel_map) {
  vertex_data_.clear();
  indices_.clear();

  for (int x = 0; x < SPRITE_WIDTH; ++x) {
    for (int y = 0; y < SPRITE_WIDTH; ++y) {
      for (int z = 0; z < SPRITE_WIDTH; ++z) {
        const auto &voxel_point = voxel_map[z * SPRITE_WIDTH * SPRITE_WIDTH + y * SPRITE_WIDTH + x];
        if (voxel_point.opaque) {
          build_voxel(x, y, z, voxel_point.color);
        }
      }
    }
  }
}

void SpriteVoxelizer::build_voxel(const int x, const int y, const int z, const glm::u8vec4 &color) {
  // clang-format off
  constexpr auto vertices = make_array(
    VoxelVertexData{{-0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {255, 255, 255, 255}},
    VoxelVertexData{{ 0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {255, 255, 255, 255}},
    VoxelVertexData{{ 0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {255, 255, 255, 255}},
    VoxelVertexData{{-0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {255, 255, 255, 255}},
    VoxelVertexData{{-0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {255, 255, 255, 255}},
    VoxelVertexData{{ 0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {255, 255, 255, 255}},
    VoxelVertexData{{ 0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {255, 255, 255, 255}},
    VoxelVertexData{{-0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {255, 255, 255, 255}},
    VoxelVertexData{{-0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f}, {255, 255, 255, 255}},
    VoxelVertexData{{ 0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f}, {255, 255, 255, 255}},
    VoxelVertexData{{ 0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f}, {255, 255, 255, 255}},
    VoxelVertexData{{-0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f}, {255, 255, 255, 255}},
    VoxelVertexData{{-0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, {255, 255, 255, 255}},
    VoxelVertexData{{ 0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, {255, 255, 255, 255}},
    VoxelVertexData{{ 0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, {255, 255, 255, 255}},
    VoxelVertexData{{-0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, {255, 255, 255, 255}},
    VoxelVertexData{{-0.5f, -0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {255, 255, 255, 255}},
    VoxelVertexData{{-0.5f,  0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {255, 255, 255, 255}},
    VoxelVertexData{{-0.5f,  0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {255, 255, 255, 255}},
    VoxelVertexData{{-0.5f, -0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {255, 255, 255, 255}},
    VoxelVertexData{{ 0.5f, -0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}, {255, 255, 255, 255}},
    VoxelVertexData{{ 0.5f,  0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}, {255, 255, 255, 255}},
    VoxelVertexData{{ 0.5f,  0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f}, {255, 255, 255, 255}},
    VoxelVertexData{{ 0.5f, -0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f}, {255, 255, 255, 255}});
  constexpr auto indices = make_array(
     2u,  1u,  0u,
     0u,  3u,  2u,
     4u,  5u,  6u,
     6u,  7u,  4u,
     8u,  9u, 10u,
    10u, 11u,  8u,
    14u, 13u, 12u,
    12u, 15u, 14u,
    18u, 17u, 16u,
    16u, 19u, 18u,
    20u, 21u, 22u,
    22u, 23u, 20u);
  // clang-format on

  const auto voxel_start_index = static_cast<std::uint32_t>(vertex_data_.size());

  constexpr auto half_width = SPRITE_WIDTH / 2;
  const auto float_pos_x = static_cast<GLfloat>(x - half_width);
  const auto float_pos_y = -static_cast<GLfloat>(y - half_width);
  const auto float_pos_z = static_cast<GLfloat>(z - half_width);

  std::transform(
      vertices.begin(),
      vertices.end(),
      std::back_inserter(vertex_data_),
      [float_pos_x, float_pos_y, float_pos_z, color](const auto &vertex) {
        return VoxelVertexData{
            {vertex.position.x + float_pos_x, vertex.position.y + float_pos_y, vertex.position.z + float_pos_z},
            vertex.normal,
            color
        };
      });
  std::transform(indices.begin(), indices.end(), std::back_inserter(indices_), [voxel_start_index](const auto index) {
    return voxel_start_index + index;
  });
}
} // namespace wolf
