#pragma once

#include <SDL2/SDL_stdinc.h>

#include <memory>

namespace tools::sdl {
class SDLKeyboardState {
public:
  SDLKeyboardState(const SDLKeyboardState &) = default;
  SDLKeyboardState(SDLKeyboardState &&) noexcept = default;
  SDLKeyboardState &operator=(const SDLKeyboardState &) = default;
  SDLKeyboardState &operator=(SDLKeyboardState &&) noexcept = default;
  ~SDLKeyboardState() = default;

  SDLKeyboardState();

  [[nodiscard]] bool right() const;
  [[nodiscard]] bool left() const;
  [[nodiscard]] bool down() const;
  [[nodiscard]] bool up() const;

private:
  const Uint8 *const sdl_keys_{nullptr};

  [[nodiscard]] const Uint8 *const sdl_keys() const;
};
} // namespace tools::sdl
