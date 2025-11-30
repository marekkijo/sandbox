#pragma once

#include <gp/misc/keyboard_state.hpp>
#include <gp/sdl/internal/sdl_context.hpp>
#include <gp/sdl/sdl.hpp>

#include <memory>
#include <string>

namespace gp::sdl::internal {
class SDLWindow {
public:
  SDLWindow(std::shared_ptr<SDLContext> ctx,
            const int width,
            const int height,
            const std::string &title,
            SDL_WindowFlags flags = default_window_flags);
  ~SDLWindow();

  SDLWindow(SDLWindow &&) = delete;
  SDLWindow &operator=(SDLWindow &&) = delete;
  SDLWindow(const SDLWindow &) = delete;
  SDLWindow &operator=(const SDLWindow &) = delete;

  static constexpr SDL_WindowFlags default_window_flags = SDL_WINDOW_RESIZABLE;

  SDL_Window *wnd() const;
  std::shared_ptr<misc::KeyboardState> keyboard_state() const;
  void set_window_event_callback(SDLWindowEventCallback callback);

private:
  const std::shared_ptr<SDLContext> ctx_{};
  SDL_Window *wnd_{};
  std::shared_ptr<misc::KeyboardState> keyboard_state_{std::make_shared<misc::KeyboardState>()};
  SDLWindowEventCallback window_event_callback_{};

  Uint32 wnd_id() const;
};
} // namespace gp::sdl::internal
