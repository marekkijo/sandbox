#pragma once

#include "tools/sdl/sdl_system.hpp"

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>

namespace wolf {
class Renderer {
public:
  Renderer(const Renderer &)                = default;
  Renderer &operator=(const Renderer &)     = default;
  Renderer(Renderer &&) noexcept            = default;
  Renderer &operator=(Renderer &&) noexcept = default;
  virtual ~Renderer()                       = default;

  Renderer(tools::sdl::SDLSystem &sdl_sys);

  virtual void animate(std::uint32_t time_elapsed_ms);
  virtual void redraw() = 0;
  void         resize();

protected:
  [[nodiscard]] SDL_Window   *wnd() const;
  [[nodiscard]] SDL_Renderer *r() const;
  virtual void                resize(int width, int height) = 0;

private:
  SDL_Window   *window_{nullptr};
  SDL_Renderer *renderer_{nullptr};
};
} // namespace wolf
