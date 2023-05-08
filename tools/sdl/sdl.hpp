#pragma once

#include "sdl_keyboard_state.hpp"

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>

#include <memory>

namespace tools::sdl {
class SDLRenderer {
public:
  SDLRenderer(const SDLRenderer &) = delete;
  SDLRenderer &operator=(const SDLRenderer &) = delete;

  SDLRenderer(SDL_Window *window, int index, Uint32 flags);
  SDLRenderer(SDLRenderer &&other) noexcept;
  SDLRenderer &operator=(SDLRenderer &&other) noexcept;
  ~SDLRenderer();

  [[nodiscard]] SDL_Renderer *renderer();

private:
  SDL_Renderer *renderer_{nullptr};
};

class SDLWindow {
public:
  SDLWindow(const SDLWindow &) = delete;
  SDLWindow &operator=(const SDLWindow &) = delete;

  SDLWindow(const char *title, int x, int y, int w, int h, Uint32 flags);
  SDLWindow(SDLWindow &&other) noexcept;
  SDLWindow &operator=(SDLWindow &&other) noexcept;
  ~SDLWindow();

  [[nodiscard]] SDL_Window *window();

private:
  SDL_Window *window_{nullptr};
};

class SDLSystem {
public:
  SDLSystem() = delete;
  SDLSystem(const SDLSystem &) = delete;
  SDLSystem &operator=(const SDLSystem &) = delete;

  SDLSystem(Uint32 sys_flags, const char *wnd_title, int wnd_x, int wnd_y, int wnd_w, int wnd_h, Uint32 wnd_flags,
            int renderer_index, Uint32 renderer_flags);
  SDLSystem(SDLSystem &&other) noexcept;
  SDLSystem &operator=(SDLSystem &&other) noexcept;
  ~SDLSystem();

  [[nodiscard]] bool ready() const;
  [[nodiscard]] SDL_Window *window();
  [[nodiscard]] SDL_Renderer *renderer();

private:
  bool ready_{false};
  bool should_quit_{false};
  std::unique_ptr<SDLWindow> window_{nullptr};
  std::unique_ptr<SDLRenderer> renderer_{nullptr};
};
} // namespace tools::sdl
