#pragma once

#include <SDL2/SDL_events.h>

#include <gp/misc/event.hpp>

namespace gp::sdl {
misc::Event to_event(const SDL_Event &sdl_event);
} // namespace gp::sdl
