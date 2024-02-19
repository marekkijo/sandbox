#pragma once

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_stdinc.h>

#include <atomic>
#include <chrono>
#include <cstdint>
#include <thread>

namespace gp::sdl {
class Animation {
public:
  Animation(const Animation &) = default;
  Animation(Animation &&) noexcept = default;
  Animation &operator=(const Animation &) = default;
  Animation &operator=(Animation &&) noexcept = default;

  Animation(std::uint16_t fps, Sint32 code = 0, void *data1 = nullptr, void *data2 = nullptr);
  ~Animation();

private:
  std::atomic_bool should_break_{false};
  std::thread thread_{};

  static void tick(std::uint16_t fps, Sint32 code, void *data1, void *data2, std::atomic_bool &should_break);
};
} // namespace gp::sdl
