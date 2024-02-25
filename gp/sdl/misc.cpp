#include "misc.hpp"

namespace gp::sdl {
misc::Event to_event(const SDL_Event &sdl_event) {
  auto event = misc::Event{};

  switch (sdl_event.type) {
  case SDL_MOUSEBUTTONDOWN:
  case SDL_MOUSEBUTTONUP:
    event.type = misc::Event::Type::MouseButton;
    event.timestamp = sdl_event.button.timestamp;

    switch (sdl_event.type) {
    case SDL_MOUSEBUTTONDOWN:
      event.mouse_button.action = misc::Event::Action::Pressed;
      break;
    case SDL_MOUSEBUTTONUP:
      event.mouse_button.action = misc::Event::Action::Released;
      break;
    default:
      event.mouse_button.action = misc::Event::Action::None;
      break;
    }
    switch (sdl_event.button.button) {
    case SDL_BUTTON_LEFT:
      event.mouse_button.button = misc::Event::MouseButton::Left;
      break;
    case SDL_BUTTON_MIDDLE:
      event.mouse_button.button = misc::Event::MouseButton::Middle;
      break;
    case SDL_BUTTON_RIGHT:
      event.mouse_button.button = misc::Event::MouseButton::Right;
      break;
    default:
      event.mouse_button.button = misc::Event::MouseButton::None;
      break;
    }
    break;
  case SDL_MOUSEMOTION:
    event.type = misc::Event::Type::MouseMove;
    event.timestamp = sdl_event.motion.timestamp;

    event.mouse_move.x = sdl_event.motion.x;
    event.mouse_move.y = sdl_event.motion.y;
    event.mouse_move.x_rel = sdl_event.motion.xrel;
    event.mouse_move.y_rel = sdl_event.motion.yrel;
    event.mouse_move.mouse_button_mask = misc::Event::MouseButtonMask::None;
    if (sdl_event.motion.state & SDL_BUTTON_LMASK) {
      event.mouse_move.mouse_button_mask |= misc::Event::MouseButtonMask::Left;
    }
    if (sdl_event.motion.state & SDL_BUTTON_MMASK) {
      event.mouse_move.mouse_button_mask |= misc::Event::MouseButtonMask::Middle;
    }
    if (sdl_event.motion.state & SDL_BUTTON_RMASK) {
      event.mouse_move.mouse_button_mask |= misc::Event::MouseButtonMask::Right;
    }
    break;
  case SDL_MOUSEWHEEL:
    event.type = misc::Event::Type::MouseScroll;
    event.timestamp = sdl_event.wheel.timestamp;

    event.mouse_scroll.horizontal = sdl_event.wheel.preciseX;
    event.mouse_scroll.vertical = sdl_event.wheel.preciseY;
    break;
  case SDL_KEYDOWN:
  case SDL_KEYUP:
    event.type = misc::Event::Type::Key;
    event.timestamp = sdl_event.key.timestamp;

    switch (sdl_event.type) {
    case SDL_KEYDOWN:
      event.key.action = misc::Event::Action::Pressed;
      break;
    case SDL_KEYUP:
      event.key.action = misc::Event::Action::Released;
      break;
    default:
      event.key.action = misc::Event::Action::None;
      break;
    }
    switch (sdl_event.key.keysym.scancode) {
    case SDL_SCANCODE_DOWN:
      event.key.code = misc::Event::KeyCode::Down;
      break;
    case SDL_SCANCODE_LEFT:
      event.key.code = misc::Event::KeyCode::Left;
      break;
    case SDL_SCANCODE_RIGHT:
      event.key.code = misc::Event::KeyCode::Right;
      break;
    case SDL_SCANCODE_UP:
      event.key.code = misc::Event::KeyCode::Up;
      break;
    case SDL_SCANCODE_ESCAPE:
      event.key.code = misc::Event::KeyCode::Escape;
      break;
    default:
      event.key.code = misc::Event::KeyCode::None;
      break;
    }
    break;
  default:
    break;
  }

  return event;
}
} // namespace gp::sdl
