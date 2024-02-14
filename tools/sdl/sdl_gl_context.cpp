#include "sdl_gl_context.hpp"

#include <stdexcept>
#include <string>

namespace tools::sdl {
SDLGLContext::SDLGLContext(SDL_Window *window) : gl_context_{SDL_GL_CreateContext(window)} {
  if (!gl()) { throw std::runtime_error{std::string{"SDL_GL_CreateContext error:"} + SDL_GetError()}; }
}

SDLGLContext::SDLGLContext(SDLGLContext &&other) noexcept : gl_context_{other.gl_context_} {
  other.gl_context_ = nullptr;
}

SDLGLContext &SDLGLContext::operator=(SDLGLContext &&other) noexcept {
  if (this == &other) { return *this; }

  SDL_GL_DeleteContext(gl_context_);

  gl_context_       = other.gl_context_;
  other.gl_context_ = nullptr;

  return *this;
}

SDLGLContext::~SDLGLContext() { SDL_GL_DeleteContext(gl_context_); }

SDL_GLContext SDLGLContext::gl() { return gl_context_; }
} // namespace tools::sdl
