#pragma once

#include <cstdint>

namespace tools::common {
struct UserInput {
  std::uint32_t type{};
  std::uint32_t timestamp{};
  std::uint32_t state{};
  std::int32_t x{};
  std::int32_t y{};
  std::int32_t x_relative{};
  std::int32_t y_relative{};
  std::uint8_t button{};
  std::uint8_t clicks{};
  float x_float{};
  float y_float{};
  std::uint8_t repeat{};
  std::uint16_t keysym_scancode{};
  std::int32_t keysym_sym{};
  std::uint16_t keysym_mod{};
};

struct UserInput2 {
  using TypeSubtype = std::uint32_t;

  enum class Type : TypeSubtype {
    None,
    MouseMotion,
    MouseButtonDown,
    MouseButtonUp,
    MouseWheel,
    KeyDown,
    KeyUp
  };

  using MaskSubtype = std::uint32_t;

  enum Mask : MaskSubtype {
    None = 0x0,
    LeftMouseButton = 0x1,
    RightMouseButton = 0x2,
    MiddleMouseButton = 0x4
  };

  Type type{Type::None};
  std::uint32_t timestamp{};
  std::uint32_t state{};
  double x{};
  double y{};
  double x_relative{};
  double y_relative{};
  std::uint8_t button{};
  std::uint8_t repeat{};
  std::int32_t keycode{};
};
} // namespace tools::common
