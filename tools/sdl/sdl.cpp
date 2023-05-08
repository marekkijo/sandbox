#include "sdl.hpp"

#include <SDL2/SDL.h>
#include <stdexcept>

namespace tools::sdl {
SDLRenderer::SDLRenderer(SDL_Window *window, int index, Uint32 flags)
    : renderer_{SDL_CreateRenderer(window, index, flags)} {
  if (!renderer())
    throw std::runtime_error{std::string{"SDL CreateRenderer Error:"} + SDL_GetError()};
}

SDLRenderer::SDLRenderer(SDLRenderer &&other) noexcept : renderer_{other.renderer_} { other.renderer_ = nullptr; }

SDLRenderer &SDLRenderer::operator=(SDLRenderer &&other) noexcept {
  if (this == &other)
    return *this;

  SDL_DestroyRenderer(renderer_);

  renderer_ = other.renderer_;
  other.renderer_ = nullptr;

  return *this;
}

SDLRenderer::~SDLRenderer() { SDL_DestroyRenderer(renderer_); }

SDL_Renderer *SDLRenderer::renderer() { return renderer_; }

SDLWindow::SDLWindow(const char *title, int x, int y, int w, int h, Uint32 flags)
    : window_{SDL_CreateWindow(title, x, y, w, h, flags)} {
  if (!window())
    throw std::runtime_error{std::string{"SDL CreateWindow Error:"} + SDL_GetError()};
}

SDLWindow::SDLWindow(SDLWindow &&other) noexcept : window_{other.window_} { other.window_ = nullptr; }

SDLWindow &SDLWindow::operator=(SDLWindow &&other) noexcept {
  if (this == &other)
    return *this;

  SDL_DestroyWindow(window_);

  window_ = other.window_;
  other.window_ = nullptr;

  return *this;
}

SDLWindow::~SDLWindow() { SDL_DestroyWindow(window_); }

SDL_Window *SDLWindow::window() { return window_; }

SDLSystem::SDLSystem(Uint32 sys_flags, const char *wnd_title, int wnd_x, int wnd_y, int wnd_w, int wnd_h,
                     Uint32 wnd_flags, int renderer_index, Uint32 renderer_flags)
    : ready_{SDL_Init(sys_flags) == 0}, should_quit_{true},
      window_{ready() ? std::make_unique<SDLWindow>(wnd_title, wnd_x, wnd_y, wnd_w, wnd_h, wnd_flags) : nullptr},
      renderer_{window() ? std::make_unique<SDLRenderer>(window(), renderer_index, renderer_flags) : nullptr} {
  if (!ready())
    throw std::runtime_error{std::string{"SDL Init Error:"} + SDL_GetError()};
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

bool SDLSystem::ready() const { return ready_; }

SDL_Window *SDLSystem::window() { return window_ ? window_->window() : nullptr; }

SDL_Renderer *SDLSystem::renderer() { return renderer_ ? renderer_->renderer() : nullptr; }
} // namespace tools::sdl
