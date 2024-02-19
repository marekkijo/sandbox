#pragma once

#include "gl_context.hpp"
#include "renderer.hpp"
#include "window.hpp"

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_video.h>

#include <memory>

namespace gp::sdl {
class System {
public:
  System(const System &) = delete;
  System &operator=(const System &) = delete;

  System(Uint32 sys_flags,
         const char *title,
         int x,
         int y,
         int w,
         int h,
         Uint32 wnd_flags,
         int r_index,
         Uint32 r_flags);
  System(System &&other) noexcept;
  System &operator=(System &&other) noexcept;
  ~System();

  [[nodiscard]] SDL_Window *wnd();
  [[nodiscard]] SDL_Renderer *r();
  [[nodiscard]] SDL_GLContext gl();

private:
  bool ready_{false};
  bool should_quit_{true};
  std::unique_ptr<Window> window_{};
  std::unique_ptr<Renderer> renderer_{};
  std::unique_ptr<GLContext> gl_context_{};
};
} // namespace gp::sdl
