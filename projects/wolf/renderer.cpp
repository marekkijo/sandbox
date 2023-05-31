#include "renderer.hpp"

#include <stdexcept>

namespace wolf {
Renderer::Renderer(tools::sdl::SDLSystem &sdl_sys) : window_{sdl_sys.wnd()}, renderer_{sdl_sys.r()} {
  if (!wnd()) { throw std::runtime_error{"given window is nullptr"}; }
  if (!r()) { throw std::runtime_error{"given renderer is nullptr"}; }
}

void Renderer::animate(const std::uint32_t /* time_elapsed_ms */) {}

void Renderer::resize() {
  int width, height;
  SDL_GetWindowSize(wnd(), &width, &height);
  resize(width, height);
}

SDL_Window *Renderer::wnd() const { return window_; }

SDL_Renderer *Renderer::r() const { return renderer_; }
} // namespace wolf
