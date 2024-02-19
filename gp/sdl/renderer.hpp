#pragma once

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_video.h>

#include <memory>

namespace gp::sdl {
class Renderer {
public:
  Renderer(const Renderer &) = delete;
  Renderer &operator=(const Renderer &) = delete;

  Renderer(SDL_Window *window, int index, Uint32 flags);
  Renderer(Renderer &&other) noexcept;
  Renderer &operator=(Renderer &&other) noexcept;
  ~Renderer();

  [[nodiscard]] SDL_Renderer *r();

private:
  SDL_Renderer *renderer_{};
};
} // namespace gp::sdl
