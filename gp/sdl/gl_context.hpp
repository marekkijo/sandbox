#pragma once

#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_video.h>

#include <memory>

namespace gp::sdl {
class GLContext {
public:
  GLContext(const GLContext &) = delete;
  GLContext &operator=(const GLContext &) = delete;

  GLContext(SDL_Window *window);
  GLContext(GLContext &&other) noexcept;
  GLContext &operator=(GLContext &&other) noexcept;
  ~GLContext();

  [[nodiscard]] SDL_GLContext gl();

private:
  SDL_GLContext gl_context_{};
};
} // namespace gp::sdl
