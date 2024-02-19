#include "system.hpp"

#include <SDL2/SDL.h>

#include <stdexcept>
#include <string>

namespace gp::sdl {
namespace {
bool should_create_gl_context(Uint32 wnd_flags) { return wnd_flags & SDL_WINDOW_OPENGL; }
} // namespace

System::System(Uint32 sys_flags,
               const char *title,
               int x,
               int y,
               int w,
               int h,
               Uint32 wnd_flags,
               int r_index,
               Uint32 r_flags)
    : ready_{SDL_Init(sys_flags) == 0}
    , window_{ready_ ? std::make_unique<Window>(title, x, y, w, h, wnd_flags) : nullptr}
    , renderer_{wnd() ? std::make_unique<Renderer>(wnd(), r_index, r_flags) : nullptr}
    , gl_context_{wnd() && should_create_gl_context(wnd_flags) ? std::make_unique<GLContext>(wnd()) : nullptr} {
  if (!ready_) { throw std::runtime_error{std::string{"SDL_Init error:"} + SDL_GetError()}; }
}

System::System(System &&other) noexcept
    : ready_{other.ready_}
    , should_quit_{other.should_quit_}
    , window_{std::move(other.window_)}
    , renderer_{std::move(other.renderer_)} {
  other.ready_ = false;
  other.should_quit_ = false;
}

System &System::operator=(System &&other) noexcept {
  if (this == &other) { return *this; }

  renderer_.reset();
  window_.reset();
  if (should_quit_) { SDL_Quit(); }

  ready_ = other.ready_;
  other.ready_ = false;

  should_quit_ = other.should_quit_;
  other.should_quit_ = false;

  window_ = std::move(other.window_);
  renderer_ = std::move(other.renderer_);

  return *this;
}

System::~System() {
  if (should_quit_) { SDL_Quit(); }
}

SDL_Window *System::wnd() { return window_ ? window_->wnd() : nullptr; }

SDL_Renderer *System::r() { return renderer_ ? renderer_->r() : nullptr; }

SDL_GLContext System::gl() { return gl_context_ ? gl_context_->gl() : nullptr; }
} // namespace gp::sdl