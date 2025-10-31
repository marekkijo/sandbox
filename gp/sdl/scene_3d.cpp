#include "scene_3d.hpp"

#include <gp/gl/gl.hpp>

#include <stdexcept>

namespace gp::sdl {
Scene3D::Scene3D(std::shared_ptr<internal::SDLContext> ctx)
    : ctx_{ctx ? ctx : std::make_shared<internal::SDLContext>()} {}

void Scene3D::init(const int width, const int height, const std::string &title, const bool async) {
  width_ = width;
  height_ = height;
  title_ = title;

  if (async) {
    const auto lock_guard = std::lock_guard(init_mutex_);
    init_done_ = true;
    init_cv_.notify_one();
    return;
  }

  if (wnd_) {
    return;
  }

  set_gl_hints();
  wnd_ = std::make_shared<internal::SDLWindow>(ctx_,
                                               width,
                                               height,
                                               title,
                                               internal::SDLWindow::default_window_flags | SDL_WINDOW_OPENGL);
  gl_ctx_ = std::make_unique<internal::GLContext>(wnd_);

  platform_gl_init();

  wnd_->set_window_event_callback([this](const misc::Event &event) { window_event_callback(event); });
}

Scene3D::~Scene3D() = default;

int Scene3D::exec(bool async_init) {
  if (async_init) {
    auto unique_lock = std::unique_lock(init_mutex_);
    init_cv_.wait(unique_lock, [this] { return init_done_; });
    init(width_, height_, title_);
  }

  return ctx_->exec();
}

int Scene3D::width() const { return width_; }

int Scene3D::height() const { return height_; }

std::uint32_t Scene3D::timestamp() const { return ctx_->timestamp(); }

void Scene3D::swap_buffers() const { SDL_GL_SwapWindow(wnd_->wnd()); }

void Scene3D::request_close() { ctx_->request_close(); }

std::shared_ptr<misc::KeyboardState> Scene3D::keyboard_state() const { return wnd_->keyboard_state(); }

void Scene3D::set_gl_hints() {
#ifdef __APPLE__
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#elif __EMSCRIPTEN__
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
#else
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#endif

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

#if !defined(NDEBUG) && !defined(__EMSCRIPTEN__)
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif
}

void Scene3D::platform_gl_init() {
#ifndef __EMSCRIPTEN__
  if (gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress)) == 0) {
    throw std::runtime_error{"Couldn't initialize GLAD"};
  }
#endif
}

void Scene3D::window_event_callback(const misc::Event &event) {
  loop(event);
  if (event.type() == misc::Event::Type::Resize) {
    width_ = event.resize().width;
    height_ = event.resize().height;
  }
}
} // namespace gp::sdl
