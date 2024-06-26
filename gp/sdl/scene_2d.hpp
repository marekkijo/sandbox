#pragma once

#include <gp/sdl/scene_2d_fwd.hpp>

#include <gp/misc/event_fwd.hpp>
#include <gp/misc/keyboard_state.hpp>
#include <gp/sdl/internal/sdl_context_fwd.hpp>
#include <gp/sdl/internal/sdl_window_fwd.hpp>
#include <gp/sdl/renderer.hpp>

#include <memory>
#include <string>

namespace gp::sdl {
class Scene2D {
public:
  explicit Scene2D(std::shared_ptr<internal::SDLContext> ctx = nullptr);
  virtual ~Scene2D();

  Scene2D(Scene2D &&) = delete;
  Scene2D &operator=(Scene2D &&) = delete;
  Scene2D(const Scene2D &) = delete;
  Scene2D &operator=(const Scene2D &) = delete;

  void init(const int width, const int height, const std::string &title);
  int exec();

protected:
  virtual void loop(const misc::Event &event) = 0;

  int width() const;
  int height() const;

  std::uint32_t timestamp() const;
  std::shared_ptr<const Renderer> renderer() const;
  const Renderer &r() const;
  std::shared_ptr<misc::KeyboardState> keyboard_state() const;

private:
  void window_event_callback(const misc::Event &event);

  int width_{};
  int height_{};
  std::string title_{};

  std::shared_ptr<internal::SDLContext> ctx_;
  std::unique_ptr<internal::SDLWindow> wnd_;
};
} // namespace gp::sdl
