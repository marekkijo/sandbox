#pragma once

#include "assimp.hpp"
#include "model_format.hpp"

#include <glm/glm.hpp>

#include <string>
#include <tuple>
#include <vector>

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
  void process_node(const aiScene *const scene, const aiNode *const node, glm::mat4 transformation);
  void process_mesh(const aiMesh *const mesh, const glm::mat4 &transformation);

  std::vector<std::vector<glm::vec3>> vertices_{};
  std::vector<std::vector<glm::vec3>> normals_{};
  std::vector<std::vector<std::uint32_t>> indices_{};
  std::vector<glm::vec4> colors_{};
};
