#pragma once

#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_video.h>

namespace gp::sdl {
class Window {
public:
  Window(const Window &) = delete;
  Window &operator=(const Window &) = delete;

  Window(const char *title, int x, int y, int w, int h, Uint32 flags);
  Window(Window &&other) noexcept;
  Window &operator=(Window &&other) noexcept;
  ~Window();

  [[nodiscard]] SDL_Window *wnd();

private:
  SDL_Window *window_{};
};
} // namespace gp::sdl
