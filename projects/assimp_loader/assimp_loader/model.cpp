#include "model.hpp"

#include <glm/gtx/transform.hpp>

#include <assimp/importerdesc.h>
#include <assimp/postprocess.h>

#include <algorithm>
#include <cstring>
#include <iterator>
#include <span>

namespace {
ModelFormat get_model_format(const Assimp::Importer &importer) {
  constexpr auto importer_index_property_name = "importerIndex";
  const auto importer_index = importer.GetPropertyInteger(importer_index_property_name);
  const auto importer_info = importer.GetImporterInfo(importer_index);

  if (strcmp(importer_info->mName, "Discreet 3DS Importer") == 0) { return ModelFormat::_3DS; }
  if (strcmp(importer_info->mName, "Blender 3D Importer (http://www.blender3d.org)") == 0) {
    return ModelFormat::Blender;
  }
  if (strcmp(importer_info->mName, "Collada Importer") == 0) { return ModelFormat::Collada; }
  if (strcmp(importer_info->mName, "Autodesk FBX Importer") == 0) { return ModelFormat::FBX; }
  if (strcmp(importer_info->mName, "glTF2 Importer") == 0) { return ModelFormat::glTF2; }
  if (strcmp(importer_info->mName, "Wavefront Object Importer") == 0) { return ModelFormat::OBJ; }
  if (strcmp(importer_info->mName, "Stereolithography (STL) Importer") == 0) { return ModelFormat::STL; }

  return ModelFormat::Unknown;
}

glm::mat4 to_glm(const aiMatrix4x4 &mat) {
  return {mat.a1,
          mat.b1,
          mat.c1,
          mat.d1,
          mat.a2,
          mat.b2,
          mat.c2,
          mat.d2,
          mat.a3,
          mat.b3,
          mat.c3,
          mat.d3,
          mat.a4,
          mat.b4,
          mat.c4,
          mat.d4};
}
} // namespace

Model::Model(const std::string &filename) {
  auto importer = Assimp::Importer();
  const auto scene = importer.ReadFile(filename, aiPostProcessSteps::aiProcess_Triangulate);
  if (!scene) { return; }

  model_format_ = get_model_format(importer);

  auto init_matrix = glm::mat4(1.0);
  if (model_format_ == ModelFormat::FBX) {
    init_matrix = glm::scale(glm::mat4(1.0), glm::vec3(1.0f / 100.0f, 1.0f / 100.0f, 1.0f / 100.0f));
  }
  process_node(scene, scene->mRootNode, init_matrix);
}

std::size_t Model::size() const { return vertices_.size(); }

std::tuple<const std::vector<glm::vec4> &, const std::vector<std::uint32_t> &, const glm::vec4 &>
Model::get(const std::size_t index) const {
  return {vertices_[index], indices_[index], colors_[index]};
}

void Model::process_node(const aiScene *const scene, const aiNode *const node, glm::mat4 transformation) {
  transformation = transformation * to_glm(node->mTransformation);
  for (const auto mesh_index : std::span(node->mMeshes, node->mNumMeshes)) {
    const auto mesh = scene->mMeshes[mesh_index];
    process_mesh(mesh, transformation);

    auto two_sided = false;
    auto color_applied = false;
    if (scene->HasMaterials()) {
      const auto material = scene->mMaterials[mesh->mMaterialIndex];
      auto color_diffuse = aiColor4D();
      if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color_diffuse) == aiReturn_SUCCESS) {
        colors_.emplace_back(color_diffuse.r, color_diffuse.g, color_diffuse.b, color_diffuse.a);
        color_applied = true;
      }
      material->Get(AI_MATKEY_TWOSIDED, two_sided);
    }

    if (two_sided) {
      auto &last_indices = indices_.back();
      last_indices.reserve(last_indices.size() * 2);
      const auto prev_size = last_indices.size();
      for (std::size_t i = 0; i < prev_size; i += 3u) {
        last_indices.emplace_back(last_indices[i + 2]);
        last_indices.emplace_back(last_indices[i + 1]);
        last_indices.emplace_back(last_indices[i + 0]);
      }
    }

    if (!color_applied) { colors_.emplace_back(0.75f, 0.75f, 0.75f, 1.0f); }
  }
  for (const auto child : std::span(node->mChildren, node->mNumChildren)) {
    process_node(scene, child, transformation);
  }
}

void Model::process_mesh(const aiMesh *const mesh, const glm::mat4 &transformation) {
  if (!mesh->HasFaces()) { return; }

  const auto vertices_span = std::span(mesh->mVertices, mesh->mNumVertices);

  auto vertices = std::vector<glm::vec4>{};
  vertices.reserve(vertices_span.size());

  std::transform(vertices_span.begin(),
                 vertices_span.end(),
                 std::back_inserter(vertices),
                 [&transformation](const auto &vec) {
                   return transformation * glm::vec4{vec.x, vec.y, vec.z, 1.0};
                 });
  vertices_.emplace_back(std::move(vertices));

  auto indices = std::vector<std::uint32_t>{};
  const auto faces_span = std::span(mesh->mFaces, mesh->mNumFaces);
  for (const auto &face : faces_span) {
    if (face.mNumIndices != 3u) { continue; }
    indices.insert(indices.end(), face.mIndices, face.mIndices + face.mNumIndices);
  }
  indices_.emplace_back(std::move(indices));
}
