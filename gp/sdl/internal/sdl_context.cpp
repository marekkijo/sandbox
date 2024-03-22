#include "sdl_context.hpp"

#include <gp/misc/event.hpp>
#include <gp/sdl/internal/misc.hpp>
#include <gp/sdl/sdl.hpp>

#include <stdexcept>

namespace gp::sdl::internal {
SDLContext::SDLContext() {
  if (context_created_) {
    throw std::runtime_error{"SDL context already created"};
  }
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    throw std::runtime_error{std::string{"SDL_Init error:"} + SDL_GetError()};
  }
  context_created_ = true;
}

SDLContext::~SDLContext() {
  SDL_Quit();
  context_created_ = false;
}

void SDLContext::set_window_event_callback(const std::uint32_t wnd_id, SDLWindowEventCallback callback) {
  window_event_callbacks_[wnd_id] = std::move(callback);
}

void SDLContext::withdraw_window_event_callback(const std::uint32_t wnd_id) { window_event_callbacks_.erase(wnd_id); }

std::uint32_t SDLContext::timestamp() const { return SDL_GetTicks(); }

int SDLContext::exec() const {
  {
    misc::Event event(misc::Event::Type::Init, timestamp());
    for (const auto &[wnd_id, callback] : window_event_callbacks_) {
      const auto wnd = SDL_GetWindowFromID(wnd_id);
      SDL_GetWindowSize(wnd, &event.init().width, &event.init().height);
      callback(event);
    }
  }

  bool quit_flag = false;
  int return_code = 0;
  SDL_Event sdl_event;

  while (!quit_flag) {
    if (!SDL_PollEvent(&sdl_event)) {
      const misc::Event redraw_event(misc::Event::Type::Redraw, timestamp());
      forward_event_to_all_windows(redraw_event);
      continue;
    }

    switch (sdl_event.type) {
    case SDL_QUIT:
      quit_flag = true;
      return_code = 1;
      break;
    case SDL_WINDOWEVENT: {
      const auto wnd_id = sdl_event.window.windowID;
      switch (sdl_event.window.event) {
      case SDL_WINDOWEVENT_SIZE_CHANGED: {
        misc::Event event(misc::Event::Type::Resize, timestamp());
        event.resize().width = sdl_event.window.data1;
        event.resize().height = sdl_event.window.data2;
        forward_event_to_window(wnd_id, event);
      } break;
      case SDL_WINDOWEVENT_CLOSE: {
        misc::Event event(misc::Event::Type::Quit, timestamp());
        event.quit().close_flag = 0;
        forward_event_to_window(wnd_id, event);
      } break;
      default:
        break;
      }
    } break;
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP: {
      const auto wnd_id = sdl_event.button.windowID;
      misc::Event event(misc::Event::Type::MouseButton, timestamp());
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
      forward_event_to_window(wnd_id, event);
    } break;
    case SDL_MOUSEMOTION: {
      const auto wnd_id = sdl_event.motion.windowID;
      misc::Event event(misc::Event::Type::MouseMove, timestamp());
      event.mouse_move().x = sdl_event.motion.x;
      event.mouse_move().y = sdl_event.motion.y;
      event.mouse_move().x_rel = sdl_event.motion.xrel;
      event.mouse_move().y_rel = sdl_event.motion.yrel;
      event.mouse_move().mouse_button_mask = misc::Event::MouseButtonMask::None;
      event.mouse_move().mouse_button_mask |=
          (sdl_event.motion.state & SDL_BUTTON_LMASK) ? misc::Event::MouseButtonMask::Left : 0;
      event.mouse_move().mouse_button_mask |=
          (sdl_event.motion.state & SDL_BUTTON_MMASK) ? misc::Event::MouseButtonMask::Middle : 0;
      event.mouse_move().mouse_button_mask |=
          (sdl_event.motion.state & SDL_BUTTON_RMASK) ? misc::Event::MouseButtonMask::Right : 0;
      forward_event_to_window(wnd_id, event);
    } break;
    case SDL_MOUSEWHEEL: {
      const auto wnd_id = sdl_event.wheel.windowID;
      misc::Event event(misc::Event::Type::MouseScroll, timestamp());
      event.mouse_scroll().vertical = sdl_event.wheel.preciseY;
      event.mouse_scroll().horizontal = sdl_event.wheel.preciseX;
      forward_event_to_window(wnd_id, event);
    } break;
    case SDL_KEYDOWN:
    case SDL_KEYUP: {
      const auto wnd_id = sdl_event.key.windowID;
      misc::Event event(misc::Event::Type::Key, timestamp());
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
      event.key().scan_code = to_scan_code(sdl_event.key.keysym.scancode);
      forward_event_to_window(wnd_id, event);
    } break;
    case SDL_DROPFILE: {
      const auto wnd_id = sdl_event.drop.windowID;
      misc::Event event(misc::Event::Type::DragDrop, timestamp());
      event.drag_drop().filepath = sdl_event.drop.file;
      SDL_free(sdl_event.drop.file);
      forward_event_to_window(wnd_id, event);
    } break;
    default:
      break;
    }
  }

  return return_code;
}

void SDLContext::forward_event_to_window(const std::uint32_t wnd_id, const misc::Event &event) const {
  const auto it = window_event_callbacks_.find(wnd_id);
  if (it != window_event_callbacks_.end()) {
    it->second(event);
  }
}

void SDLContext::forward_event_to_all_windows(const misc::Event &event) const {
  for (const auto &[wnd_id, callback] : window_event_callbacks_) {
    callback(event);
  }
}

bool SDLContext::context_created_{false};
} // namespace gp::sdl::internal
