#pragma once

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
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

  [[nodiscard]] SDL_Renderer *r();

private:
  SDL_Renderer *renderer_{};
};
} // namespace tools::sdl
