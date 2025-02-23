#include "model.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

#define CGLTF_IMPLEMENTATION
#include <cgltf.h>

#include <algorithm>
#include <iterator>
#include <span>
#include <stdexcept>

namespace {
glm::mat4 to_glm(const cgltf_float mat[16]) {
  return {mat[0],
          mat[1],
          mat[2],
          mat[3],
          mat[4],
          mat[5],
          mat[6],
          mat[7],
          mat[8],
          mat[9],
          mat[10],
          mat[11],
          mat[12],
          mat[13],
          mat[14],
          mat[15]};
}

std::vector<std::uint32_t> extract_scalar_vector(const cgltf_accessor *const accessor) {
  if (accessor->type != cgltf_type_scalar) {
    throw std::runtime_error("Accessor is not a scalar");
  }
  if (accessor->component_type == cgltf_component_type_r_8u) {
    auto scalars = std::vector<std::uint8_t>(accessor->count);
    void *src = reinterpret_cast<void *>(reinterpret_cast<std::byte *>(accessor->buffer_view->buffer->data) +
                                         accessor->buffer_view->offset);
    memcpy(scalars.data(), src, accessor->buffer_view->size);
    return std::vector<std::uint32_t>(scalars.begin(), scalars.end());
  }
  if (accessor->component_type == cgltf_component_type_r_16u) {
    auto scalars = std::vector<std::uint16_t>(accessor->count);
    void *src = reinterpret_cast<void *>(reinterpret_cast<std::byte *>(accessor->buffer_view->buffer->data) +
                                         accessor->buffer_view->offset);
    memcpy(scalars.data(), src, accessor->buffer_view->size);
    return std::vector<std::uint32_t>(scalars.begin(), scalars.end());
  }
  if (accessor->component_type == cgltf_component_type_r_32u) {
    auto scalars = std::vector<std::uint32_t>(accessor->count);
    void *src = reinterpret_cast<void *>(reinterpret_cast<std::byte *>(accessor->buffer_view->buffer->data) +
                                         accessor->buffer_view->offset);
    memcpy(scalars.data(), src, accessor->buffer_view->size);
    return scalars;
  }
  throw std::runtime_error("Unsupported component type");
}
} // namespace

Model::Model(const std::string &filename) {
  cgltf_options options{};
  cgltf_data *data = nullptr;
  cgltf_result result = cgltf_parse_file(&options, filename.c_str(), &data);
  if (result != cgltf_result_success) {
    return;
  }

  result = cgltf_load_buffers(&options, data, filename.c_str());
  if (result != cgltf_result_success) {
    cgltf_free(data);
    return;
  }

  result = cgltf_validate(data);
  if (result != cgltf_result_success) {
    cgltf_free(data);
    return;
  }

  auto transformation_matrix = glm::mat4(1.0f);
  for (int i = 0; i < data->nodes_count; i++) {
    process_node(data, data->nodes + i, transformation_matrix);
  }

  cgltf_free(data);
  meshes_.clear();
}

std::size_t Model::size() const { return vertices_.size(); }

std::tuple<const std::vector<glm::vec3> &,
           const std::vector<glm::vec3> &,
           const std::vector<std::uint32_t> &,
           const glm::vec4 &>
Model::get(const std::size_t index) const {
  return {vertices_[index], normals_[index], indices_[index], colors_[index]};
}

void Model::process_node(const cgltf_data *const data, const cgltf_node *const node, glm::mat4 transformation) {
  transformation = transformation * to_glm(node->matrix);
  process_mesh(node->mesh, transformation);
  for (const auto child : std::span(node->children, node->children_count)) {
    process_node(data, child, transformation);
  }
}

void Model::process_mesh(const cgltf_mesh *const mesh, const glm::mat4 &transformation) {
  if (!mesh) {
    return;
  }
  if (meshes_.contains(mesh->name)) {
    return;
  }
  meshes_.emplace(mesh->name);
  for (int i = 0; i < mesh->primitives_count; i++) {
    const auto primitive = mesh->primitives + i;
    for (int j = 0; j < primitive->attributes_count; j++) {
      const auto attrib = primitive->attributes + j;
      const auto size = attrib->data->count;
      if (attrib->type == cgltf_attribute_type_position) {
        auto vertices = std::vector<glm::vec3>(size);
        void *src = reinterpret_cast<void *>(reinterpret_cast<std::byte *>(attrib->data->buffer_view->buffer->data) +
                                             attrib->data->buffer_view->offset);
        memcpy(vertices.data(), src, attrib->data->buffer_view->size);
        vertices_.emplace_back(std::move(vertices));
      }
      if (attrib->type == cgltf_attribute_type_normal) {
        auto normals = std::vector<glm::vec3>(size);
        void *src = reinterpret_cast<void *>(reinterpret_cast<std::byte *>(attrib->data->buffer_view->buffer->data) +
                                             attrib->data->buffer_view->offset);
        memcpy(normals.data(), src, attrib->data->buffer_view->size);
        normals_.emplace_back(std::move(normals));
      }
    }

    indices_.emplace_back(extract_scalar_vector(primitive->indices));
    colors_.emplace_back(0.75f, 0.75f, 0.75f, 1.0f);
  }
}
