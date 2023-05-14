#include "sdl_renderer.hpp"

#include <stdexcept>
#include <string>

namespace tools::sdl {
SDLRenderer::SDLRenderer(SDL_Window *window, int index, Uint32 flags)
    : renderer_{SDL_CreateRenderer(window, index, flags)} {
  if (!r()) { throw std::runtime_error{std::string{"SDL CreateRenderer Error:"} + SDL_GetError()}; }
}

SDLRenderer::SDLRenderer(SDLRenderer &&other) noexcept : renderer_{other.renderer_} { other.renderer_ = nullptr; }

SDLRenderer &SDLRenderer::operator=(SDLRenderer &&other) noexcept {
  if (this == &other) { return *this; }

  SDL_DestroyRenderer(renderer_);

  renderer_       = other.renderer_;
  other.renderer_ = nullptr;

  return *this;
}

SDLRenderer::~SDLRenderer() { SDL_DestroyRenderer(renderer_); }

SDL_Renderer *SDLRenderer::r() { return renderer_; }
} // namespace tools::sdl
