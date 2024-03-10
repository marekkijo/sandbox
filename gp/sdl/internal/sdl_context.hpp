#pragma once

#include <gp/sdl/internal/sdl_context_fwd.hpp>

#include <functional>
#include <unordered_map>

namespace gp::sdl::internal {
class SDLContext {
public:
  SDLContext();
  ~SDLContext();

  SDLContext(SDLContext &&) = delete;
  SDLContext &operator=(SDLContext &&) = delete;
  SDLContext(const SDLContext &) = delete;
  SDLContext &operator=(const SDLContext &) = delete;

  void set_window_event_callback(const std::uint32_t wnd_id, SDLWindowEventCallback callback);
  void withdraw_window_event_callback(const std::uint32_t wnd_id);
  /**
   * Returns the timestamp of the SDL context.
   *
   * @return The number of milliseconds since the SDL context was created.
   */
  std::uint32_t timestamp() const;
  int exec() const;

private:
  void forward_event_to_window(const std::uint32_t wnd_id, const misc::Event &event) const;
  void forward_event_to_all_windows(const misc::Event &event) const;

  std::unordered_map<std::uint32_t, SDLWindowEventCallback> window_event_callbacks_{};

  static bool context_created_;
};
} // namespace gp::sdl::internal
