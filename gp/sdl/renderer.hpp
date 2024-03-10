#pragma once

#include <gp/sdl/internal/sdl_renderer_fwd.hpp>
#include <gp/sdl/sdl.hpp>

#include <memory>

namespace gp::sdl {
class Renderer {
public:
  explicit Renderer(std::unique_ptr<internal::SDLRenderer> r);
  ~Renderer();

  void clear() const;
  void present() const;
  void set_draw_color(const std::uint8_t r, const std::uint8_t g, const std::uint8_t b, const std::uint8_t a) const;
  void fill_rect(const SDL_Rect &rect) const;
  void fill_rects(const SDL_Rect *rects, const int count) const;
  void draw_line(const int x1, const int y1, const int x2, const int y2) const;
  void draw_line(const float x1, const float y1, const float x2, const float y2) const;

private:
  std::unique_ptr<internal::SDLRenderer> r_;
};
} // namespace gp::sdl
