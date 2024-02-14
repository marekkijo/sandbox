#pragma once

#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_video.h>

namespace tools::sdl {
class SDLWindow {
public:
  SDLWindow(const SDLWindow &) = delete;
  SDLWindow &operator=(const SDLWindow &) = delete;

  SDLWindow(const char *title, int x, int y, int w, int h, Uint32 flags);
  SDLWindow(SDLWindow &&other) noexcept;
  SDLWindow &operator=(SDLWindow &&other) noexcept;
  ~SDLWindow();

  [[nodiscard]] SDL_Window *wnd();

private:
  SDL_Window *window_{};
};
} // namespace tools::sdl
