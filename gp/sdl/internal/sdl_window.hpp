#pragma once

#include <gp/sdl/internal/sdl_window_fwd.hpp>

#include <gp/misc/keyboard_state.hpp>
#include <gp/sdl/internal/sdl_context_fwd.hpp>
#include <gp/sdl/renderer.hpp>
#include <gp/sdl/sdl.hpp>

#include <memory>
#include <string>

namespace gp::sdl::internal {
class SDLWindow {
public:
  SDLWindow(std::shared_ptr<SDLContext> ctx, const int width, const int height, const std::string &title);
  ~SDLWindow();

  SDLWindow(SDLWindow &&) = delete;
  SDLWindow &operator=(SDLWindow &&) = delete;
  SDLWindow(const SDLWindow &) = delete;
  SDLWindow &operator=(const SDLWindow &) = delete;

  SDL_Window *wnd() const;
  std::shared_ptr<const Renderer> renderer() const;
  const Renderer &r() const;
  std::shared_ptr<misc::KeyboardState> keyboard_state() const;
  void set_window_event_callback(SDLWindowEventCallback callback);

private:
  const std::shared_ptr<SDLContext> ctx_{};
  SDL_Window *wnd_{};
  std::shared_ptr<Renderer> r_{};
  std::shared_ptr<misc::KeyboardState> keyboard_state_{std::make_shared<misc::KeyboardState>()};
  SDLWindowEventCallback window_event_callback_{};
};
} // namespace gp::sdl::internal
