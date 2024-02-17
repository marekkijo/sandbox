#pragma once

#include <assimp/Importer.hpp>

enum class ModelFormat {
  unknown,
  _3ds,
  blender,
  collada,
  fbx,
  gltf2,
  obj,
  stl
};

ModelFormat get_model_format(const Assimp::Importer &importer);
