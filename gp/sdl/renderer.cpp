#include "renderer.hpp"

#include <gp/sdl/internal/sdl_renderer.hpp>

#include <SDL_ttf.h>

#include <stdexcept>

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
  SDL_SetRenderDrawBlendMode(r_->r(), SDL_BLENDMODE_BLEND);
}

void Renderer::fill_rect(const SDL_Rect &rect) const { SDL_RenderFillRect(r_->r(), &rect); }

void Renderer::fill_rects(const SDL_Rect *rects, const int count) const { SDL_RenderFillRects(r_->r(), rects, count); }

void Renderer::draw_line(const int x1, const int y1, const int x2, const int y2) const {
  SDL_RenderDrawLine(r_->r(), x1, y1, x2, y2);
}

void Renderer::draw_line(const float x1, const float y1, const float x2, const float y2) const {
  SDL_RenderDrawLineF(r_->r(), x1, y1, x2, y2);
}

// TODO: This is a temporary implementation.
//       Should be replaced within ticket https://github.com/marekkijo/sandbox/issues/31
void Renderer::draw_text(const std::string &text, const int x, const int y) const {
  SDL_Color color;
  SDL_GetRenderDrawColor(r_->r(), &color.r, &color.g, &color.b, &color.a);

  const auto result = TTF_Init();
  if (result < 0) {
    throw std::runtime_error("Couldn't initialize TTF: " + std::string(SDL_GetError()));
  }

  const auto font = TTF_OpenFont("data/Consolas.ttf", 24);
  if (!font) {
    throw std::runtime_error("Could not load font: " + std::string(TTF_GetError()));
  }
  const auto surface = TTF_RenderUTF8_Solid(font, text.c_str(), color);
  if (!surface) {
    throw std::runtime_error("Could not render text: " + std::string(TTF_GetError()));
  }

  const auto texture = SDL_CreateTextureFromSurface(r_->r(), surface);
  if (!texture) {
    throw std::runtime_error("Could not create texture: " + std::string(SDL_GetError()));
  }

  SDL_Rect rect;
  rect.x = x;
  rect.y = y;
  rect.w = surface->w;
  rect.h = surface->h;

  SDL_RenderCopy(r_->r(), texture, nullptr, &rect);

  TTF_CloseFont(font);
  SDL_FreeSurface(surface);
  SDL_DestroyTexture(texture);

  TTF_Quit();
}
} // namespace gp::sdl
