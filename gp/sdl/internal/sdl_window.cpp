#include "sdl_window.hpp"

#include <gp/misc/event.hpp>
#include <gp/sdl/internal/sdl_context.hpp>
#include <gp/sdl/internal/sdl_renderer.hpp>
#include <gp/sdl/sdl.hpp>

#include <stdexcept>

namespace gp::sdl::internal {
SDLWindow::SDLWindow(std::shared_ptr<SDLContext> ctx, const int width, const int height, const std::string &title)
    : ctx_{std::move(ctx)}
    , wnd_{SDL_CreateWindow(title.c_str(),
                            SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED,
                            width,
                            height,
                            SDL_WINDOW_SHOWN)} {
  if (!wnd()) {
    throw std::runtime_error{std::string{"SDL_CreateWindow error:"} + SDL_GetError()};
  }

  auto r = std::make_unique<SDLRenderer>(*this);
  r_ = std::make_shared<Renderer>(std::move(r));
}

SDLWindow::~SDLWindow() {
  const auto wnd_id = SDL_GetWindowID(wnd());
  ctx_->withdraw_window_event_callback(wnd_id);
  r_.reset();
  SDL_DestroyWindow(wnd());
}

SDL_Window *SDLWindow::wnd() const { return wnd_; }

std::shared_ptr<const Renderer> SDLWindow::renderer() const { return r_; }

const Renderer &SDLWindow::r() const { return *r_; }

std::shared_ptr<misc::KeyboardState> SDLWindow::keyboard_state() const { return keyboard_state_; }

void SDLWindow::set_window_event_callback(SDLWindowEventCallback callback) {
  const auto wnd_id = SDL_GetWindowID(wnd());
  ctx_->set_window_event_callback(wnd_id, [this](const misc::Event &event) {
    if (event.type() == misc::Event::Type::Key) {
      keyboard_state_->process_key_event(event.key());
    }
    if (window_event_callback_) {
      window_event_callback_(event);
    }
  });

  window_event_callback_ = std::move(callback);
}
} // namespace gp::sdl::internal
