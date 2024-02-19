#include "animation.hpp"

#include <stdexcept>

namespace gp::sdl {
Animation::Animation(std::uint16_t fps, Sint32 code, void *data1, void *data2)
    : thread_{tick, fps, code, data1, data2, std::ref(should_break_)} {}

Animation::~Animation() {
  should_break_ = true;
  thread_.join();
}

void Animation::tick(std::uint16_t fps, Sint32 code, void *data1, void *data2, std::atomic_bool &should_break) {
  const auto sleep_duration = std::chrono::milliseconds{1000} / fps;
  auto user_event = SDL_Event{SDL_USEREVENT};
  user_event.user.code = code;
  user_event.user.data1 = data1;
  user_event.user.data2 = data2;

  while (!should_break) {
    std::this_thread::sleep_for(sleep_duration);
    if (!SDL_PushEvent(&user_event)) { throw std::runtime_error{"animation event push failed"}; }
  }
}
} // namespace gp::sdl
