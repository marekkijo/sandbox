#include "scene_2d.hpp"

#include <gp/misc/event.hpp>
#include <gp/sdl/internal/sdl_context.hpp>
#include <gp/sdl/internal/sdl_window.hpp>

#include <stdexcept>

namespace gp::sdl {
Scene2D::Scene2D(std::shared_ptr<internal::SDLContext> ctx)
    : ctx_{ctx ? ctx : std::make_shared<internal::SDLContext>()} {}

void Scene2D::init(const int width, const int height, const std::string &title) {
  if (wnd_) { return; }
  wnd_ = std::make_unique<internal::SDLWindow>(ctx_, width, height, title);

  wnd_->set_window_event_callback([this](const misc::Event &event) { window_event_callback(event); });

  misc::Event event(misc::Event::Type::Init, ctx_->timestamp());
  event.init().width = width;
  event.init().height = height;
  loop(event);
}

Scene2D::~Scene2D() = default;

int Scene2D::exec() { return ctx_->exec(); }

int Scene2D::width() const { return width_; }

int Scene2D::height() const { return height_; }

std::shared_ptr<internal::SDLContext> Scene2D::ctx() { return ctx_; }

void Scene2D::window_event_callback(const misc::Event &event) { loop(event); }
} // namespace gp::sdl
