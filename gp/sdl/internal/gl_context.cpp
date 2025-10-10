#include "gl_context.hpp"

#include <gp/sdl/internal/sdl_window.hpp>

#include <stdexcept>

namespace gp::sdl::internal {
GLContext::GLContext(std::shared_ptr<SDLWindow> win)
    : win_{std::move(win)}
    , gl_ctx_{SDL_GL_CreateContext(win_->wnd())} {
  if (!gl_ctx()) {
    throw std::runtime_error{std::string{"SDL_GL_CreateContext error:"} + SDL_GetError()};
  }

  make_current();
}

GLContext::~GLContext() {
  if (gl_ctx()) {
    SDL_GL_DeleteContext(gl_ctx());
    gl_ctx_ = nullptr;
  }
}

void GLContext::make_current() const {
  if (SDL_GL_MakeCurrent(win_->wnd(), gl_ctx()) != 0) {
    throw std::runtime_error{std::string{"SDL_GL_MakeCurrent error:"} + SDL_GetError()};
  }
}

SDL_GLContext GLContext::gl_ctx() const { return gl_ctx_; }
} // namespace gp::sdl::internal
