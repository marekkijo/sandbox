#include "renderer.hpp"

#include <gp/sdl/internal/sdl_renderer.hpp>

namespace gp::sdl {
Renderer::Renderer(std::unique_ptr<internal::SDLRenderer> r)
    : r_(std::move(r)) {}

Renderer::~Renderer() = default;

void Renderer::clear() const { SDL_RenderClear(r_->r()); }

void Renderer::present() const { SDL_RenderPresent(r_->r()); }

void Renderer::set_draw_color(const std::uint8_t r,
                              const std::uint8_t g,
                              const std::uint8_t b,
                              const std::uint8_t a) const {
  SDL_SetRenderDrawColor(r_->r(), r, g, b, a);
}

void Renderer::fill_rect(const SDL_Rect &rect) const { SDL_RenderFillRect(r_->r(), &rect); }

void Renderer::fill_rects(const SDL_Rect *rects, const int count) const { SDL_RenderFillRects(r_->r(), rects, count); }

void Renderer::draw_line(const int x1, const int y1, const int x2, const int y2) const {
  SDL_RenderDrawLine(r_->r(), x1, y1, x2, y2);
}

void Renderer::draw_line(const float x1, const float y1, const float x2, const float y2) const {
  SDL_RenderDrawLineF(r_->r(), x1, y1, x2, y2);
}
} // namespace gp::sdl
