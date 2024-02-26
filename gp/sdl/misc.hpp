#pragma once

#include <gp/misc/event.hpp>

#include <SDL2/SDL_events.h>

namespace gp::sdl {
misc::Event to_event(const SDL_Event &sdl_event);
} // namespace gp::sdl
