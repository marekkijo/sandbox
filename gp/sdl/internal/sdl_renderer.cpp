#include "sdl_renderer.hpp"

#include <gp/sdl/internal/sdl_window.hpp>

#include <stdexcept>

namespace gp::sdl::internal {
SDLRenderer::SDLRenderer(const SDLWindow &wnd)
    : r_{SDL_CreateRenderer(wnd.wnd(), -1, SDL_RENDERER_ACCELERATED)} {
  if (!r()) {
    throw std::runtime_error{std::string{"SDL_CreateRenderer error:"} + SDL_GetError()};
  }
}

SDLRenderer::~SDLRenderer() { SDL_DestroyRenderer(r()); }

SDL_Renderer *SDLRenderer::r() const { return r_; }
} // namespace gp::sdl::internal
