#include "sdl_keyboard_state.hpp"

#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_scancode.h>

#include <stdexcept>

namespace tools::sdl {
SDLKeyboardState::SDLKeyboardState() : sdl_keys_{SDL_GetKeyboardState(nullptr)} {
  if (!sdl_keys_) { throw std::runtime_error{"retrieving SDL keyboard state failed"}; }
}

bool SDLKeyboardState::right() const { return sdl_keys()[SDL_SCANCODE_RIGHT] == 1u; }

bool SDLKeyboardState::left() const { return sdl_keys()[SDL_SCANCODE_LEFT] == 1u; }

bool SDLKeyboardState::down() const { return sdl_keys()[SDL_SCANCODE_DOWN] == 1u; }

bool SDLKeyboardState::up() const { return sdl_keys()[SDL_SCANCODE_UP] == 1u; }

const Uint8 *const SDLKeyboardState::sdl_keys() const { return sdl_keys_; }
} // namespace tools::sdl
