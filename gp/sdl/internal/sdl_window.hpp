#pragma once

#include <gp/sdl/internal/sdl_window_fwd.hpp>

#include <gp/sdl/internal/sdl_context_fwd.hpp>

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

  void set_window_event_callback(SDLWindowEventCallback callback);

private:
  struct InternalData;
  const std::unique_ptr<InternalData> data_;

  const std::shared_ptr<SDLContext> ctx_{};
  SDLWindowEventCallback window_event_callback_{};
};
} // namespace gp::sdl::internal
