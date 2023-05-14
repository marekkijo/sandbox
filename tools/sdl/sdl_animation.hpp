#pragma once

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_stdinc.h>

#include <atomic>
#include <chrono>
#include <cstdint>
#include <thread>

namespace tools::sdl {
class SDLAnimation {
public:
  SDLAnimation(const SDLAnimation &)                = default;
  SDLAnimation(SDLAnimation &&) noexcept            = default;
  SDLAnimation &operator=(const SDLAnimation &)     = default;
  SDLAnimation &operator=(SDLAnimation &&) noexcept = default;

  SDLAnimation(std::uint16_t fps, Sint32 code = 0, void *data1 = nullptr, void *data2 = nullptr);
  ~SDLAnimation();

private:
  std::atomic_bool should_break_{false};
  std::thread      thread_{};

  static void tick(std::uint16_t fps, Sint32 code, void *data1, void *data2, std::atomic_bool &should_break);
};
} // namespace tools::sdl
