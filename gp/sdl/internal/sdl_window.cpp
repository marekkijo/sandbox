#include "sdl_window.hpp"

#include <gp/misc/event.hpp>
#include <gp/sdl/internal/sdl_context.hpp>
#include <gp/sdl/sdl.hpp>

#include <stdexcept>

namespace gp::sdl::internal {
struct SDLWindow::InternalData {
  SDL_Window *wnd{};
  SDL_Renderer *renderer{};
};

SDLWindow::SDLWindow(std::shared_ptr<SDLContext> ctx, const int width, const int height, const std::string &title)
    : data_{std::make_unique<InternalData>()}
    , ctx_{std::move(ctx)} {
  data_->wnd =
      SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
  if (!data_->wnd) { throw std::runtime_error{std::string{"SDL_CreateWindow error:"} + SDL_GetError()}; }

  data_->renderer = SDL_CreateRenderer(data_->wnd, -1, SDL_RENDERER_ACCELERATED);
  if (!data_->renderer) {
    SDL_DestroyWindow(data_->wnd);
    throw std::runtime_error{std::string{"SDL_CreateRenderer error:"} + SDL_GetError()};
  }
}

SDLWindow::~SDLWindow() {
  const auto wnd_id = SDL_GetWindowID(data_->wnd);
  ctx_->withdraw_window_event_callback(wnd_id);

  SDL_DestroyRenderer(data_->renderer);
  SDL_DestroyWindow(data_->wnd);
}

void SDLWindow::set_window_event_callback(SDLWindowEventCallback callback) {
  const auto wnd_id = SDL_GetWindowID(data_->wnd);
  ctx_->set_window_event_callback(wnd_id, [this](const misc::Event &event) {
    if (window_event_callback_) {
      window_event_callback_(event);
      if (event.type() == misc::Event::Type::Redraw) { SDL_RenderPresent(data_->renderer); }
    }
  });

  window_event_callback_ = std::move(callback);
}
} // namespace gp::sdl::internal
