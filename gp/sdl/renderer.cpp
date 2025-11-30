#include "renderer.hpp"

#include <gp/sdl/internal/sdl_renderer.hpp>
#include <gp/sdl/sdl.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/compatibility.hpp>

#include <algorithm>
#include <iterator>
#include <stdexcept>

namespace gp::sdl {
namespace {
uint8_t lerp(const uint8_t start, const uint8_t end, const float t) {
  const auto d = static_cast<float>(end) - start;
  return static_cast<uint8_t>(glm::clamp(start + t * d, 0.0f, 255.0f));
}

glm::uvec3 lerp(const glm::uvec3 &start, const glm::uvec3 &end, const float t) {
  return glm::uvec3{lerp(start.r, end.r, t), lerp(start.g, end.g, t), lerp(start.b, end.b, t)};
}

glm::uvec4 lerp(const glm::uvec4 &start, const glm::uvec4 &end, const float t) {
  return glm::uvec4{lerp(start.r, end.r, t), lerp(start.g, end.g, t), lerp(start.b, end.b, t), lerp(start.a, end.a, t)};
}

SDL_FRect make_sdl_rect(const glm::vec2 &center, const float size = 1.0f) {
  return {center.x - size / 2.0f, center.y - size / 2.0f, size, size};
}

glm::uvec4 to_u8(const glm::vec4 &v) { return glm::uvec4(glm::round(glm::clamp(v, 0.0f, 1.0f) * 255.0f)); }
} // namespace

Renderer::Renderer(std::unique_ptr<internal::SDLRenderer> r)
    : r_(std::move(r)) {}

Renderer::~Renderer() = default;

void Renderer::clear() const { SDL_RenderClear(r_->r()); }

void Renderer::present() const { SDL_RenderPresent(r_->r()); }

void Renderer::set_color(const std::uint8_t r, const std::uint8_t g, const std::uint8_t b) const {
  set_color(glm::uvec4{r, g, b, 255u});
}

void Renderer::set_color(const std::uint8_t r, const std::uint8_t g, const std::uint8_t b, const std::uint8_t a) const {
  set_color(glm::uvec4{r, g, b, a});
}

void Renderer::set_color(const glm::uvec3 &color) const { set_color(glm::uvec4{color, 255u}); }

void Renderer::set_color(const glm::uvec4 &color) const {
  SDL_SetRenderDrawColor(r_->r(), color.r, color.g, color.b, color.a);
  SDL_SetRenderDrawBlendMode(r_->r(), SDL_BLENDMODE_BLEND);
}

void Renderer::set_color(const glm::vec3 &color) const { set_color(to_u8(glm::vec4{color, 1.0f})); }

void Renderer::set_color(const glm::vec4 &color) const { set_color(to_u8(color)); }

void Renderer::draw_rect(const SDL_FRect &rect) const { SDL_RenderRect(r_->r(), &rect); }

void Renderer::draw_rects(const SDL_FRect *rects, const int count) const { SDL_RenderRects(r_->r(), rects, count); }

void Renderer::draw_rects(const std::vector<SDL_FRect> &rects) const {
  draw_rects(rects.data(), static_cast<int>(rects.size()));
}

void Renderer::fill_rect(const SDL_FRect &rect) const { SDL_RenderFillRect(r_->r(), &rect); }

void Renderer::fill_rects(const SDL_FRect *rects, const int count) const { SDL_RenderFillRects(r_->r(), rects, count); }

void Renderer::fill_rects(const std::vector<SDL_FRect> &rects) const {
  fill_rects(rects.data(), static_cast<int>(rects.size()));
}

void Renderer::draw_rect_at(const glm::vec2 &center, const float size) const { draw_rect(make_sdl_rect(center, size)); }

void Renderer::fill_rect_at(const glm::vec2 &center, const float size) const { fill_rect(make_sdl_rect(center, size)); }

void Renderer::draw_line(const float x1, const float y1, const float x2, const float y2) const {
  draw_line(glm::vec2{x1, y1}, glm::vec2{x2, y2});
}

void Renderer::draw_line(const glm::vec2 &start, const glm::vec2 &end) const {
  SDL_RenderLine(r_->r(), start.x, start.y, end.x, end.y);
}

void Renderer::draw_gradient_line(const glm::vec2 &start,
                                  const glm::uvec3 &start_color,
                                  const glm::vec2 &end,
                                  const glm::uvec3 &end_color,
                                  const unsigned int segments) const {
  draw_gradient_line(start, glm::uvec4{start_color, 255u}, end, glm::uvec4{end_color, 255u}, segments);
}

void Renderer::draw_gradient_line(const glm::vec2 &start,
                                  const glm::uvec4 &start_color,
                                  const glm::vec2 &end,
                                  const glm::uvec4 &end_color,
                                  const unsigned int segments) const {
  const auto dx = end.x - start.x;
  const auto dy = end.y - start.y;

  auto prev_pt = start;
  for (int i = 1; i <= segments; ++i) {
    const auto t = static_cast<float>(i) / static_cast<float>(segments);
    const auto mid_pt = glm::lerp(start, end, t);
    const auto mid_color = lerp(start_color, end_color, t);
    set_color(mid_color);
    draw_line(prev_pt, mid_pt);
    prev_pt = mid_pt;
  }
}

void Renderer::draw_geometry(const std::vector<glm::vec2> &points) const {
  SDL_FColor rgba;
  SDL_GetRenderDrawColorFloat(r_->r(), &rgba.r, &rgba.g, &rgba.b, &rgba.a);
  std::vector<SDL_Vertex> vertices;
  vertices.reserve(points.size());
  std::transform(points.begin(), points.end(), std::back_inserter(vertices), [&rgba](const glm::vec2 &p) {
    return SDL_Vertex{
        SDL_FPoint{p.x, p.y},
        rgba
    };
  });
  SDL_RenderGeometry(r_->r(), nullptr, vertices.data(), static_cast<int>(vertices.size()), nullptr, 0);
}

// TODO: This is a temporary implementation.
//       Should be replaced within ticket https://github.com/marekkijo/sandbox/issues/31
void Renderer::draw_text(const std::string &text, const int x, const int y) const {
  SDL_Color color;
  SDL_GetRenderDrawColor(r_->r(), &color.r, &color.g, &color.b, &color.a);

  if (!TTF_Init()) {
    throw std::runtime_error("Couldn't initialize TTF: " + std::string(SDL_GetError()));
  }

  const auto font = TTF_OpenFont("data/Consolas.ttf", 24);
  if (!font) {
    throw std::runtime_error("Could not load font: " + std::string(SDL_GetError()));
  }
  const auto surface = TTF_RenderText_Solid(font, text.c_str(), 0, color);
  if (!surface) {
    throw std::runtime_error("Could not render text: " + std::string(SDL_GetError()));
  }

  const auto texture = SDL_CreateTextureFromSurface(r_->r(), surface);
  if (!texture) {
    throw std::runtime_error("Could not create texture: " + std::string(SDL_GetError()));
  }

  SDL_FRect rect;
  rect.x = static_cast<float>(x);
  rect.y = static_cast<float>(y);
  rect.w = static_cast<float>(surface->w);
  rect.h = static_cast<float>(surface->h);

  SDL_RenderTexture(r_->r(), texture, nullptr, &rect);

  TTF_CloseFont(font);
  SDL_DestroySurface(surface);
  SDL_DestroyTexture(texture);

  TTF_Quit();
}
} // namespace gp::sdl
