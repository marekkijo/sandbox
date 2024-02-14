#include "model_orientation.hpp"

namespace {
bool operator==(const Orientation &lhs, const Orientation &rhs) { return lhs.up == rhs.up && lhs.front == rhs.front; }

Axis to_axis(const std::int32_t data_axis, const std::int32_t data_sign) {
  if (data_sign == -1) {
    if (data_axis == 0) { return Axis::x_minus; }
    if (data_axis == 1) { return Axis::y_minus; }
    return Axis::z_minus;
  } else {
    if (data_axis == 0) { return Axis::x_plus; }
    if (data_axis == 1) { return Axis::y_plus; }
    return Axis::z_plus;
  }
}

Orientation orientation_from_metadata(const Orientation &default_orientation, const aiMetadata *const metadata) {
  if (!metadata) { return default_orientation; }

  auto data_axis = std::int32_t{};
  auto data_sign = std::int32_t{};

  auto orientation = Orientation{};

  if (!metadata->Get("UpAxis", data_axis)) { return default_orientation; }
  if (!metadata->Get("UpAxisSign", data_sign)) { return default_orientation; }
  orientation.up = to_axis(data_axis, data_sign);

  if (!metadata->Get("FrontAxis", data_axis)) { return default_orientation; }
  if (!metadata->Get("FrontAxisSign", data_sign)) { return default_orientation; }
  orientation.front = to_axis(data_axis, -data_sign);

  return orientation;
}
} // namespace

Orientation get_orientation(const ModelFormat model_format, const aiMetadata *const metadata) {
  auto orientation = Orientation{};

  switch (model_format) {
  case ModelFormat::blender:
  case ModelFormat::stl:
    orientation = Orientation{Axis::z_plus, Axis::y_minus};
    break;
  case ModelFormat::_3ds:
  case ModelFormat::collada:
  case ModelFormat::gltf2:
  case ModelFormat::obj:
    orientation = Orientation{Axis::y_plus, Axis::z_plus};
    break;
  case ModelFormat::fbx:
  case ModelFormat::unknown:
    break;
  }

  return orientation_from_metadata(orientation, metadata);
}

glm::mat4 get_orientation_matrix(const Orientation &orientation) {
  if (orientation == Orientation{} /* Axis::y_plus, Axis::z_minus */) { return glm::mat4(1.0); }
  if (orientation == Orientation{Axis::y_minus, Axis::z_plus}) {
    return {1, 0, 0, 0, 0, -1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 1};
  }
  if (orientation == Orientation{Axis::y_plus, Axis::z_plus}) {
    return {-1, 0, 0, 0, 0, 1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 1};
  }
  if (orientation == Orientation{Axis::y_minus, Axis::z_minus}) {
    return {-1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
  }
  if (orientation == Orientation{Axis::z_minus, Axis::y_plus}) {
    return {-1, 0, 0, 0, 0, 0, -1, 0, 0, -1, 0, 0, 0, 0, 0, 1};
  }
  if (orientation == Orientation{Axis::z_plus, Axis::y_minus}) {
    return {-1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1};
  }
  if (orientation == Orientation{Axis::z_minus, Axis::y_minus}) {
    return {1, 0, 0, 0, 0, 0, 1, 0, 0, -1, 0, 0, 0, 0, 0, 1};
  }
  if (orientation == Orientation{Axis::z_plus, Axis::y_plus}) {
    return {1, 0, 0, 0, 0, 0, -1, 0, 0, 1, 0, 0, 0, 0, 0, 1};
  }
  if (orientation == Orientation{Axis::z_plus, Axis::x_minus}) {
    return {0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1};
  }
  if (orientation == Orientation{Axis::z_minus, Axis::x_plus}) {
    return {0, 0, -1, 0, 1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 1};
  }
  if (orientation == Orientation{Axis::z_plus, Axis::x_plus}) {
    return {0, 0, -1, 0, -1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1};
  }
  if (orientation == Orientation{Axis::z_minus, Axis::x_minus}) {
    return {0, 0, 1, 0, -1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 1};
  }
  if (orientation == Orientation{Axis::x_minus, Axis::z_plus}) {
    return {0, -1, 0, 0, -1, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 1};
  }
  if (orientation == Orientation{Axis::x_plus, Axis::z_minus}) {
    return {0, 1, 0, 0, -1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
  }
  if (orientation == Orientation{Axis::x_minus, Axis::z_minus}) {
    return {0, -1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
  }
  if (orientation == Orientation{Axis::x_plus, Axis::z_plus}) {
    return {0, 1, 0, 0, 1, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 1};
  }
  if (orientation == Orientation{Axis::x_plus, Axis::y_minus}) {
    return {0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1};
  }
  if (orientation == Orientation{Axis::x_minus, Axis::y_plus}) {
    return {0, -1, 0, 0, 0, 0, -1, 0, 1, 0, 0, 0, 0, 0, 0, 1};
  }
  if (orientation == Orientation{Axis::x_plus, Axis::y_plus}) {
    return {0, 1, 0, 0, 0, 0, -1, 0, -1, 0, 0, 0, 0, 0, 0, 1};
  }
  if (orientation == Orientation{Axis::x_minus, Axis::y_minus}) {
    return {0, -1, 0, 0, 0, 0, 1, 0, -1, 0, 0, 0, 0, 0, 0, 1};
  }
  if (orientation == Orientation{Axis::y_minus, Axis::x_plus}) {
    return {0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, 0, 0, 0, 0, 1};
  }
  if (orientation == Orientation{Axis::y_plus, Axis::x_minus}) {
    return {0, 0, 1, 0, 0, 1, 0, 0, -1, 0, 0, 0, 0, 0, 0, 1};
  }
  if (orientation == Orientation{Axis::y_minus, Axis::x_minus}) {
    return {0, 0, 1, 0, 0, -1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1};
  }
  if (orientation == Orientation{Axis::y_plus, Axis::x_plus}) {
    return {0, 0, -1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1};
  }
  return glm::mat4(1.0);
}
