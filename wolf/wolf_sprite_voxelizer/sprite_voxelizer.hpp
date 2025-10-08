#pragma once

#include "wolf_common/vswap_file.hpp"

#include <gp/gl/gl.hpp>

#include <glm/glm.hpp>

#include <memory>
#include <vector>

class SpriteVoxelizer {
public:
#pragma pack(push, 1)

  struct VoxelVertexData {
    glm::vec3 position{};
    glm::vec3 normal{};
    glm::u8vec4 color{};
  };

#pragma pack(pop)

  explicit SpriteVoxelizer(std::shared_ptr<const wolf::VswapFile> vswap_file);
  ~SpriteVoxelizer() = default;

  void voxelize(const std::size_t front_sprite_index,
                const std::size_t back_sprite_index,
                const std::size_t left_sprite_index,
                const std::size_t right_sprite_index);
  void voxelize(const std::size_t sprite_index, const bool mirrored_sides);

  const std::vector<VoxelVertexData> &vertex_data() const;
  const std::vector<GLuint> &indices() const;

private:
  struct VoxelPoint {
    glm::u8vec4 color{};
    bool opaque{false};
  };

  std::vector<VoxelPoint> prepare_voxel_map(const std::size_t front_sprite_index,
                                            const std::size_t back_sprite_index,
                                            const std::size_t left_sprite_index,
                                            const std::size_t right_sprite_index);
  std::vector<VoxelPoint> prepare_voxel_map(const std::size_t sprite_index, const bool mirrored_sides);
  void build_voxel_model(const std::vector<VoxelPoint> &voxel_map);
  void build_voxel(const int x, const int y, const int z, const glm::u8vec4 &color);

  std::shared_ptr<const wolf::VswapFile> vswap_file_{};
  std::vector<VoxelVertexData> vertex_data_{};
  std::vector<GLuint> indices_{};
};
