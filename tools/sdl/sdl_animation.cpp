#include "sdl_animation.hpp"

#include <stdexcept>

namespace tools::sdl {
SDLAnimation::SDLAnimation(std::uint16_t fps, Sint32 code, void *data1, void *data2)
    : thread_{tick, fps, code, data1, data2, std::ref(should_break_)} {}

SDLAnimation::~SDLAnimation() {
  should_break_ = true;
  thread_.join();
}

void SDLAnimation::tick(std::uint16_t fps, Sint32 code, void *data1, void *data2, std::atomic_bool &should_break) {
  const auto sleep_duration_ms = std::chrono::milliseconds{1000} / fps;
  auto user_event = SDL_Event{SDL_USEREVENT};
  user_event.user.code = code;
  user_event.user.data1 = data1;
  user_event.user.data2 = data2;

  while (!should_break) {
    std::this_thread::sleep_for(sleep_duration_ms);
    if (!SDL_PushEvent(&user_event)) { throw std::runtime_error{"animation event push failed"}; }
  }
}
} // namespace tools::sdl
