#pragma once

#include <gp/sdl/internal/sdl_renderer_fwd.hpp>
#include <gp/sdl/sdl.hpp>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <memory>
#include <string>
#include <vector>

namespace gp::sdl {
class Renderer {
public:
  explicit Renderer(std::unique_ptr<internal::SDLRenderer> r);
  ~Renderer();

  void clear() const;

  void present() const;

  void set_color(const std::uint8_t r, const std::uint8_t g, const std::uint8_t b) const;
  void set_color(const std::uint8_t r, const std::uint8_t g, const std::uint8_t b, const std::uint8_t a) const;
  void set_color(const glm::uvec3 &color) const;
  void set_color(const glm::uvec4 &color) const;
  void set_color(const glm::vec3 &color) const;
  void set_color(const glm::vec4 &color) const;

  void draw_rect(const SDL_FRect &rect) const;
  void draw_rects(const SDL_FRect *rects, const int count) const;
  void draw_rects(const std::vector<SDL_FRect> &rects) const;

  void fill_rect(const SDL_FRect &rect) const;
  void fill_rects(const SDL_FRect *rects, const int count) const;
  void fill_rects(const std::vector<SDL_FRect> &rects) const;

  void draw_rect_at(const glm::vec2 &center, const float size = 1.0f) const;
  void fill_rect_at(const glm::vec2 &center, const float size = 1.0f) const;

  void draw_line(const float x1, const float y1, const float x2, const float y2) const;
  void draw_line(const glm::vec2 &start, const glm::vec2 &end) const;

  void draw_gradient_line(const glm::vec2 &start,
                          const glm::uvec3 &start_color,
                          const glm::vec2 &end,
                          const glm::uvec3 &end_color,
                          const unsigned int segments = default_segments) const;
  void draw_gradient_line(const glm::vec2 &start,
                          const glm::uvec4 &start_color,
                          const glm::vec2 &end,
                          const glm::uvec4 &end_color,
                          const unsigned int segments = default_segments) const;

  void draw_geometry(const std::vector<glm::vec2> &points) const;

  void draw_text(const std::string &text, const int x, const int y) const;

  static constexpr auto default_segments = 8u;

private:
  std::unique_ptr<internal::SDLRenderer> r_;
};
} // namespace gp::sdl
