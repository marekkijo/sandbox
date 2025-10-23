#pragma once

#include <gp/sdl/internal/sdl_window.hpp>
#include <gp/sdl/sdl.hpp>

#include <memory>

namespace gp::sdl::internal {
class GLContext {
public:
  GLContext(std::shared_ptr<SDLWindow> win);
  ~GLContext();

  SDL_GLContext gl_ctx() const;
  void make_current() const;

private:
  const std::shared_ptr<SDLWindow> win_{};
  SDL_GLContext gl_ctx_{};
};
} // namespace gp::sdl::internal
