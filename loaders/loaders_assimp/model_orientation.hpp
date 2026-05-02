#pragma once

#include "model_format.hpp"

#include <assimp/metadata.h>
#include <glm/glm.hpp>

enum class Axis {
  x_minus = 0,
  y_minus = 1,
  z_minus = 2,
  x_plus = 3,
  y_plus = 4,
  z_plus = 5
};

struct Orientation {
  Axis up{Axis::y_plus};
  Axis front{Axis::z_minus};
};

Orientation get_orientation(const ModelFormat model_format, const aiMetadata *const metadata);

glm::mat4 get_orientation_matrix(const Orientation &orientation);
