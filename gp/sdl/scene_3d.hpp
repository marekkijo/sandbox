#pragma once

#include <gp/misc/event_fwd.hpp>
#include <gp/misc/keyboard_state.hpp>
#include <gp/sdl/internal/gl_context.hpp>
#include <gp/sdl/internal/sdl_context_fwd.hpp>
#include <gp/sdl/internal/sdl_window_fwd.hpp>
#include <gp/sdl/renderer.hpp>

#include <memory>
#include <string>

namespace gp::sdl {
class Scene3D {
public:
  explicit Scene3D(std::shared_ptr<internal::SDLContext> ctx = nullptr);
  virtual ~Scene3D();

  void init(const int width, const int height, const std::string &title);
  int exec();

protected:
  virtual void loop(const misc::Event &event) = 0;

  int width() const;
  int height() const;

  std::uint32_t timestamp() const;

  void swap_buffers() const;
  std::shared_ptr<const Renderer> renderer() const;
  const Renderer &r() const;
  std::shared_ptr<misc::KeyboardState> keyboard_state() const;

private:
  void set_gl_hints();
  void platform_gl_init();
  void window_event_callback(const misc::Event &event);

  int width_{};
  int height_{};
  std::string title_{};

  std::shared_ptr<internal::SDLContext> ctx_;
  std::shared_ptr<internal::SDLWindow> wnd_;
  std::unique_ptr<internal::GLContext> gl_ctx_;
};
} // namespace gp::sdl
