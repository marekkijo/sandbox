#pragma once

#include <SDL2/SDL_events.h>

namespace gp::sdl {
enum class UserEvent : std::uint16_t {
  Init = SDL_USEREVENT,
  Redraw
};
} // namespace gp::sdl
