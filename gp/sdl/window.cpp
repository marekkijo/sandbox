#include "window.hpp"

#include <stdexcept>
#include <string>

namespace gp::sdl {
Window::Window(const char *title, int x, int y, int w, int h, Uint32 flags)
    : window_{SDL_CreateWindow(title, x, y, w, h, flags)} {
  if (!wnd()) { throw std::runtime_error{std::string{"SDL_CreateWindow error:"} + SDL_GetError()}; }
}

Window::Window(Window &&other) noexcept
    : window_{other.window_} {
  other.window_ = nullptr;
}

Window &Window::operator=(Window &&other) noexcept {
  if (this == &other) { return *this; }

  SDL_DestroyWindow(window_);

  window_ = other.window_;
  other.window_ = nullptr;

  return *this;
}

Window::~Window() { SDL_DestroyWindow(window_); }

SDL_Window *Window::wnd() { return window_; }
} // namespace gp::sdl
