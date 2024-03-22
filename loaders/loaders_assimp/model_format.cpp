#include "model_format.hpp"

#include <cstring>

ModelFormat get_model_format(const Assimp::Importer &importer) {
  constexpr auto importer_index_property_name = "importerIndex";
  const auto importer_index = importer.GetPropertyInteger(importer_index_property_name);
  if (importer_index != 0xffffffff) {
    const auto importer_info = importer.GetImporterInfo(importer_index);

    if (strcmp(importer_info->mName, "Discreet 3DS Importer") == 0) {
      return ModelFormat::_3ds;
    }
    if (strcmp(importer_info->mName, "Blender 3D Importer (http://www.blender3d.org)") == 0) {
      return ModelFormat::blender;
    }
    if (strcmp(importer_info->mName, "Collada Importer") == 0) {
      return ModelFormat::collada;
    }
    if (strcmp(importer_info->mName, "Autodesk FBX Importer") == 0) {
      return ModelFormat::fbx;
    }
    if (strcmp(importer_info->mName, "glTF2 Importer") == 0) {
      return ModelFormat::gltf2;
    }
    if (strcmp(importer_info->mName, "Wavefront Object Importer") == 0) {
      return ModelFormat::obj;
    }
    if (strcmp(importer_info->mName, "Stereolithography (STL) Importer") == 0) {
      return ModelFormat::stl;
    }
  }

  return ModelFormat::unknown;
}
