#pragma once

#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_video.h>

#include <memory>

namespace tools::sdl {
class SDLGLContext {
public:
  SDLGLContext(const SDLGLContext &)            = delete;
  SDLGLContext &operator=(const SDLGLContext &) = delete;

  SDLGLContext(SDL_Window *window);
  SDLGLContext(SDLGLContext &&other) noexcept;
  SDLGLContext &operator=(SDLGLContext &&other) noexcept;
  ~SDLGLContext();

  [[nodiscard]] SDL_GLContext gl();

private:
  SDL_GLContext gl_context_{};
};
} // namespace tools::sdl
