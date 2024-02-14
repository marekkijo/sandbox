#include "sdl_window.hpp"

#include <stdexcept>
#include <string>

namespace tools::sdl {
SDLWindow::SDLWindow(const char *title, int x, int y, int w, int h, Uint32 flags)
    : window_{SDL_CreateWindow(title, x, y, w, h, flags)} {
  if (!wnd()) { throw std::runtime_error{std::string{"SDL_CreateWindow error:"} + SDL_GetError()}; }
}

SDLWindow::SDLWindow(SDLWindow &&other) noexcept : window_{other.window_} { other.window_ = nullptr; }

SDLWindow &SDLWindow::operator=(SDLWindow &&other) noexcept {
  if (this == &other) { return *this; }

  SDL_DestroyWindow(window_);

  window_       = other.window_;
  other.window_ = nullptr;

  return *this;
}

SDLWindow::~SDLWindow() { SDL_DestroyWindow(window_); }

SDL_Window *SDLWindow::wnd() { return window_; }
} // namespace tools::sdl
