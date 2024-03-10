#pragma once

#include <gp/misc/event.hpp>
#include <gp/sdl/sdl.hpp>

namespace gp::sdl::internal {
misc::Event::ScanCode to_scan_code(const SDL_Scancode sdl_scan_code);
} // namespace gp::sdl::internal
