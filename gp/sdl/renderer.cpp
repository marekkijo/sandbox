#include "renderer.hpp"

#include <stdexcept>
#include <string>

namespace gp::sdl {
Renderer::Renderer(SDL_Window *window, int index, Uint32 flags)
    : renderer_{SDL_CreateRenderer(window, index, flags)} {
  if (!r()) { throw std::runtime_error{std::string{"SDL_CreateRenderer error:"} + SDL_GetError()}; }
}

Renderer::Renderer(Renderer &&other) noexcept
    : renderer_{other.renderer_} {
  other.renderer_ = nullptr;
}

Renderer &Renderer::operator=(Renderer &&other) noexcept {
  if (this == &other) { return *this; }

  SDL_DestroyRenderer(renderer_);

  renderer_ = other.renderer_;
  other.renderer_ = nullptr;

  return *this;
}

Renderer::~Renderer() { SDL_DestroyRenderer(renderer_); }

SDL_Renderer *Renderer::r() { return renderer_; }
} // namespace gp::sdl
