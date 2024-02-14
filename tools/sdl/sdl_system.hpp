#pragma once

#include "sdl_gl_context.hpp"
#include "sdl_renderer.hpp"
#include "sdl_window.hpp"

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_video.h>

#include <memory>

namespace tools::sdl {
class SDLSystem {
public:
  SDLSystem(const SDLSystem &)            = delete;
  SDLSystem &operator=(const SDLSystem &) = delete;

  SDLSystem(Uint32      sys_flags,
            const char *title,
            int         x,
            int         y,
            int         w,
            int         h,
            Uint32      wnd_flags,
            int         r_index,
            Uint32      r_flags);
  SDLSystem(SDLSystem &&other) noexcept;
  SDLSystem &operator=(SDLSystem &&other) noexcept;
  ~SDLSystem();

  [[nodiscard]] SDL_Window   *wnd();
  [[nodiscard]] SDL_Renderer *r();
  [[nodiscard]] SDL_GLContext gl();

private:
  bool                          ready_{false};
  bool                          should_quit_{true};
  std::unique_ptr<SDLWindow>    window_{};
  std::unique_ptr<SDLRenderer>  renderer_{};
  std::unique_ptr<SDLGLContext> gl_context_{};
};
} // namespace tools::sdl
