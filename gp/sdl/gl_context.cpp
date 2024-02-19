#include "gl_context.hpp"

#include <stdexcept>
#include <string>

namespace gp::sdl {
GLContext::GLContext(SDL_Window *window)
    : gl_context_{SDL_GL_CreateContext(window)} {
  if (!gl()) { throw std::runtime_error{std::string{"SDL_GL_CreateContext error:"} + SDL_GetError()}; }
}

GLContext::GLContext(GLContext &&other) noexcept
    : gl_context_{other.gl_context_} {
  other.gl_context_ = nullptr;
}

GLContext &GLContext::operator=(GLContext &&other) noexcept {
  if (this == &other) { return *this; }

  SDL_GL_DeleteContext(gl_context_);

  gl_context_ = other.gl_context_;
  other.gl_context_ = nullptr;

  return *this;
}

GLContext::~GLContext() { SDL_GL_DeleteContext(gl_context_); }

SDL_GLContext GLContext::gl() { return gl_context_; }
} // namespace gp::sdl
