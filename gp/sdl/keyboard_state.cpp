#include "keyboard_state.hpp"

#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_scancode.h>

#include <stdexcept>

namespace gp::sdl {
KeyboardState::KeyboardState()
    : sdl_keys_{SDL_GetKeyboardState(nullptr)} {
  if (!sdl_keys_) { throw std::runtime_error{"retrieving SDL keyboard state failed"}; }
}

bool KeyboardState::right() const { return sdl_keys()[SDL_SCANCODE_RIGHT] == 1u; }

bool KeyboardState::left() const { return sdl_keys()[SDL_SCANCODE_LEFT] == 1u; }

bool KeyboardState::down() const { return sdl_keys()[SDL_SCANCODE_DOWN] == 1u; }

bool KeyboardState::up() const { return sdl_keys()[SDL_SCANCODE_UP] == 1u; }

const Uint8 *const KeyboardState::sdl_keys() const { return sdl_keys_; }
} // namespace gp::sdl
