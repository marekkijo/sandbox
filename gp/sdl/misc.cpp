#include "misc.hpp"

#include <gp/sdl/user_event.hpp>

#include <stdexcept>

namespace gp::sdl {
namespace {
misc::Event::Type from_sdl(const SDL_Event &sdl_event) {
  if (static_cast<UserEvent>(sdl_event.type) == UserEvent::Init) { return misc::Event::Type::Init; }
  if (sdl_event.type == SDL_QUIT) { return misc::Event::Type::Quit; }
  if (sdl_event.type == SDL_WINDOWEVENT && sdl_event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
    return misc::Event::Type::Resize;
  }
  if (static_cast<UserEvent>(sdl_event.type) == UserEvent::Redraw) { return misc::Event::Type::Redraw; }
  if (sdl_event.type == SDL_MOUSEBUTTONDOWN || sdl_event.type == SDL_MOUSEBUTTONUP) {
    return misc::Event::Type::MouseButton;
  }
  if (sdl_event.type == SDL_MOUSEMOTION) { return misc::Event::Type::MouseMove; }
  if (sdl_event.type == SDL_MOUSEWHEEL) { return misc::Event::Type::MouseScroll; }
  if (sdl_event.type == SDL_KEYDOWN || sdl_event.type == SDL_KEYUP) { return misc::Event::Type::Key; }
  if (sdl_event.type == SDL_DROPFILE) { return misc::Event::Type::DragDrop; }
  return misc::Event::Type::None;
}

std::uint32_t extract_timestamp(const SDL_Event &sdl_event, misc::Event::Type type) {
  switch (type) {
  case misc::Event::Type::Init:
    return sdl_event.user.timestamp;
  case misc::Event::Type::Quit:
    return sdl_event.quit.timestamp;
  case misc::Event::Type::Resize:
    return sdl_event.window.timestamp;
  case misc::Event::Type::Redraw:
    return sdl_event.user.timestamp;
  case misc::Event::Type::MouseButton:
    return sdl_event.button.timestamp;
  case misc::Event::Type::MouseMove:
    return sdl_event.motion.timestamp;
  case misc::Event::Type::MouseScroll:
    return sdl_event.wheel.timestamp;
  case misc::Event::Type::Key:
    return sdl_event.key.timestamp;
  case misc::Event::Type::DragDrop:
    return sdl_event.drop.timestamp;
  default:
    return 0u;
  }
}
} // namespace

misc::Event to_event(const SDL_Event &sdl_event) {
  const auto type = from_sdl(sdl_event);
  const auto timestamp = extract_timestamp(sdl_event, type);
  auto event = misc::Event(type, timestamp);

  switch (event.type()) {
  case misc::Event::Type::Init:
    event.init() = *reinterpret_cast<const misc::Event::InitData *>(sdl_event.user.data1);
    break;
  case misc::Event::Type::Quit:
    event.quit().close_flag = 0;
    break;
  case misc::Event::Type::Resize:
    event.resize().width = sdl_event.window.data1;
    event.resize().height = sdl_event.window.data2;
    break;
  case misc::Event::Type::Redraw:
    break;
  case misc::Event::Type::MouseButton:
    switch (sdl_event.type) {
    case SDL_MOUSEBUTTONDOWN:
      event.mouse_button().action = misc::Event::Action::Pressed;
      break;
    case SDL_MOUSEBUTTONUP:
      event.mouse_button().action = misc::Event::Action::Released;
      break;
    default:
      event.mouse_button().action = misc::Event::Action::None;
      break;
    }
    switch (sdl_event.button.button) {
    case SDL_BUTTON_LEFT:
      event.mouse_button().button = misc::Event::MouseButton::Left;
      break;
    case SDL_BUTTON_MIDDLE:
      event.mouse_button().button = misc::Event::MouseButton::Middle;
      break;
    case SDL_BUTTON_RIGHT:
      event.mouse_button().button = misc::Event::MouseButton::Right;
      break;
    default:
      event.mouse_button().button = misc::Event::MouseButton::None;
      break;
    }
    break;
  case misc::Event::Type::MouseMove:
    event.mouse_move().x = sdl_event.motion.x;
    event.mouse_move().y = sdl_event.motion.y;
    event.mouse_move().x_rel = sdl_event.motion.xrel;
    event.mouse_move().y_rel = sdl_event.motion.yrel;
    event.mouse_move().mouse_button_mask = misc::Event::MouseButtonMask::None;
    if (sdl_event.motion.state & SDL_BUTTON_LMASK) {
      event.mouse_move().mouse_button_mask |= misc::Event::MouseButtonMask::Left;
    }
    if (sdl_event.motion.state & SDL_BUTTON_MMASK) {
      event.mouse_move().mouse_button_mask |= misc::Event::MouseButtonMask::Middle;
    }
    if (sdl_event.motion.state & SDL_BUTTON_RMASK) {
      event.mouse_move().mouse_button_mask |= misc::Event::MouseButtonMask::Right;
    }
    break;
  case misc::Event::Type::MouseScroll:
    event.mouse_scroll().vertical = sdl_event.wheel.preciseY;
    event.mouse_scroll().horizontal = sdl_event.wheel.preciseX;
    break;
  case misc::Event::Type::Key:
    switch (sdl_event.type) {
    case SDL_KEYDOWN:
      event.key().action = misc::Event::Action::Pressed;
      break;
    case SDL_KEYUP:
      event.key().action = misc::Event::Action::Released;
      break;
    default:
      event.key().action = misc::Event::Action::None;
      break;
    }
    switch (sdl_event.key.keysym.scancode) {
    case SDL_SCANCODE_DOWN:
      event.key().code = misc::Event::KeyCode::Down;
      break;
    case SDL_SCANCODE_LEFT:
      event.key().code = misc::Event::KeyCode::Left;
      break;
    case SDL_SCANCODE_RIGHT:
      event.key().code = misc::Event::KeyCode::Right;
      break;
    case SDL_SCANCODE_UP:
      event.key().code = misc::Event::KeyCode::Up;
      break;
    case SDL_SCANCODE_ESCAPE:
      event.key().code = misc::Event::KeyCode::Escape;
      break;
    default:
      event.key().code = misc::Event::KeyCode::None;
      break;
    }
    break;
  case misc::Event::Type::DragDrop:
    event.drag_drop() = *reinterpret_cast<const misc::Event::DragDropData *>(sdl_event.user.data1);
    break;
  default:
    throw std::runtime_error("Unhandled event type occurred");
    break;
  }

  return event;
}
} // namespace gp::sdl
