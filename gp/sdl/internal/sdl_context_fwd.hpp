#pragma once

#include <gp/misc/event_fwd.hpp>

#include <functional>

namespace gp::sdl::internal {
class SDLContext;
using SDLWindowEventCallback = std::function<void(const misc::Event &)>;
} // namespace gp::sdl::internal
