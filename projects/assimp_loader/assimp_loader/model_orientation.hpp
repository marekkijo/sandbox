#pragma once

#include <glm/glm.hpp>

#include <assimp/Importer.hpp>
#include <assimp/metadata.h>

#include "model_format.hpp"

enum class Axis {
  x_minus,
  y_minus,
  z_minus,
  x_plus,
  y_plus,
  z_plus
};

struct Orientation {
  Axis up{Axis::y_plus};
  Axis front{Axis::z_minus};
};

Orientation get_orientation(const ModelFormat model_format, const aiMetadata *const metadata);

glm::mat4 get_orientation_matrix(const Orientation &orientation);
