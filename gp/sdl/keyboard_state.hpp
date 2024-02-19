#pragma once

#include <SDL2/SDL_stdinc.h>

#include <memory>

namespace gp::sdl {
class KeyboardState {
public:
  KeyboardState(const KeyboardState &) = default;
  KeyboardState(KeyboardState &&) noexcept = default;
  KeyboardState &operator=(const KeyboardState &) = default;
  KeyboardState &operator=(KeyboardState &&) noexcept = default;
  ~KeyboardState() = default;

  KeyboardState();

  [[nodiscard]] bool right() const;
  [[nodiscard]] bool left() const;
  [[nodiscard]] bool down() const;
  [[nodiscard]] bool up() const;

private:
  const Uint8 *const sdl_keys_{nullptr};

  [[nodiscard]] const Uint8 *const sdl_keys() const;
};
} // namespace gp::sdl
