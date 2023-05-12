#include "sdl_system.hpp"

#include <SDL2/SDL.h>

#include <stdexcept>
#include <string>

namespace tools::sdl {
SDLSystem::SDLSystem(Uint32 sys_flags, const char *title, int x, int y, int w, int h, Uint32 wnd_flags, int r_index,
                     Uint32 r_flags)
    : ready_{SDL_Init(sys_flags) == 0},
      window_{ready_ ? std::make_unique<SDLWindow>(title, x, y, w, h, wnd_flags) : nullptr},
      renderer_{wnd() ? std::make_unique<SDLRenderer>(wnd(), r_index, r_flags) : nullptr} {
  if (!ready_) {
    throw std::runtime_error{std::string{"SDL Init Error:"} + SDL_GetError()};
  }
}

SDLSystem::SDLSystem(SDLSystem &&other) noexcept
    : ready_{other.ready_}, should_quit_{other.should_quit_}, window_{std::move(other.window_)},
      renderer_{std::move(other.renderer_)} {
  other.ready_ = false;
  other.should_quit_ = false;
}

SDLSystem &SDLSystem::operator=(SDLSystem &&other) noexcept {
  if (this == &other)
    return *this;

  renderer_.reset();
  window_.reset();
  if (should_quit_)
    SDL_Quit();

  ready_ = other.ready_;
  other.ready_ = false;
  should_quit_ = other.should_quit_;
  other.should_quit_ = false;
  window_ = std::move(other.window_);
  renderer_ = std::move(other.renderer_);

  return *this;
}

SDLSystem::~SDLSystem() {
  if (should_quit_)
    SDL_Quit();
}

SDL_Window *SDLSystem::wnd() { return window_ ? window_->wnd() : nullptr; }

SDL_Renderer *SDLSystem::r() { return renderer_ ? renderer_->r() : nullptr; }
} // namespace tools::sdl
