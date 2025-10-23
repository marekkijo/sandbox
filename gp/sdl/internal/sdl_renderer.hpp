#pragma once

#include <gp/sdl/internal/sdl_renderer_fwd.hpp>

#include <gp/sdl/internal/sdl_window.hpp>
#include <gp/sdl/sdl.hpp>

namespace gp::sdl::internal {
class SDLRenderer {
public:
  explicit SDLRenderer(const SDLWindow &wnd);
  ~SDLRenderer();

  SDLRenderer(SDLRenderer &&) = delete;
  SDLRenderer &operator=(SDLRenderer &&) = delete;
  SDLRenderer(const SDLRenderer &) = delete;
  SDLRenderer &operator=(const SDLRenderer &) = delete;

  SDL_Renderer *r() const;

private:
  SDL_Renderer *r_{};
};
} // namespace gp::sdl::internal
