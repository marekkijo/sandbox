#pragma once

#include <cgltf.h>
#include <glm/glm.hpp>

#include <string>
#include <tuple>
#include <vector>
#include <set>

class Model {
public:
  Model(const Model &) = delete;
  Model &operator=(const Model &) = delete;
  Model(Model &&other) noexcept = delete;
  Model &operator=(Model &&other) noexcept = delete;

  explicit Model(const std::string &filename);
  ~Model() = default;

  std::size_t size() const;
  std::tuple<const std::vector<glm::vec3> &,
             const std::vector<glm::vec3> &,
             const std::vector<std::uint32_t> &,
             const glm::vec4 &>
  get(const std::size_t index) const;

private:
  void process_node(const cgltf_data *const scene, const cgltf_node *const node, glm::mat4 transformation);
  void process_mesh(const cgltf_mesh *const mesh, const glm::mat4 &transformation);

  std::vector<std::vector<glm::vec3>> vertices_{};
  std::vector<std::vector<glm::vec3>> normals_{};
  std::vector<std::vector<std::uint32_t>> indices_{};
  std::vector<glm::vec4> colors_{};
  std::set<std::string> meshes_{};
};
