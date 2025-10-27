#include "scene_2d.hpp"

namespace gp::sdl {
Scene2D::Scene2D(std::shared_ptr<internal::SDLContext> ctx)
    : ctx_{ctx ? ctx : std::make_shared<internal::SDLContext>()} {}

void Scene2D::init(const int width, const int height, const std::string &title) {
  if (wnd_) {
    return;
  }
  wnd_ = std::make_unique<internal::SDLWindow>(ctx_, width, height, title);

  wnd_->set_window_event_callback([this](const misc::Event &event) { window_event_callback(event); });

  width_ = width;
  height_ = height;
}

Scene2D::~Scene2D() = default;

int Scene2D::exec() { return ctx_->exec(); }

int Scene2D::width() const { return width_; }

int Scene2D::height() const { return height_; }

std::uint32_t Scene2D::timestamp() const { return ctx_->timestamp(); }

void Scene2D::request_close() { ctx_->request_close(); }

std::shared_ptr<const Renderer> Scene2D::renderer() const { return wnd_->renderer(); }

const Renderer &Scene2D::r() const { return wnd_->r(); }

std::shared_ptr<misc::KeyboardState> Scene2D::keyboard_state() const { return wnd_->keyboard_state(); }

void Scene2D::window_event_callback(const misc::Event &event) {
  loop(event);
  if (event.type() == misc::Event::Type::Resize) {
    width_ = event.resize().width;
    height_ = event.resize().height;
  } else if (event.type() == misc::Event::Type::Quit) {
    wnd_.reset();
  }
}
} // namespace gp::sdl
