#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>

#include <tools/math/math.hpp>
#include <tools/sdl/sdl.hpp>

#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <memory>
#include <numbers>
#include <thread>
#include <vector>

namespace wolf {
class Map {
public:
  Map() {
    map_.resize(20);

    // clang-format off
    map_[ 0] = {-1.0f,  1.0f, -0.5f,  1.0f};
    map_[ 1] = {-0.5f,  1.0f,  0.0f,  1.0f};
    map_[ 2] = { 0.0f,  1.0f,  0.5f,  1.0f};
    map_[ 3] = { 0.5f,  1.0f,  1.0f,  1.0f};

    map_[ 4] = { 1.0f,  1.0f,  1.0f,  0.5f};
    map_[ 5] = { 1.0f,  0.5f,  1.0f,  0.0f};
    map_[ 6] = { 1.0f,  0.0f,  1.0f, -0.5f};
    map_[ 7] = { 1.0f, -0.5f,  1.0f, -1.0f};

    map_[ 8] = { 1.0f, -1.0f,  0.5f, -1.0f};
    map_[ 9] = { 0.5f, -1.0f,  0.0f, -1.0f};
    map_[10] = { 0.0f, -1.0f, -0.5f, -1.0f};
    map_[11] = {-0.5f, -1.0f, -1.0f, -1.0f};

    map_[12] = {-1.0f, -1.0f, -1.0f, -0.5f};
    map_[13] = {-1.0f, -0.5f, -1.0f,  0.0f};
    map_[14] = {-1.0f,  0.0f, -1.0f,  0.5f};
    map_[15] = {-1.0f,  0.5f, -1.0f,  1.0f};

    map_[16] = {0.25f, 0.25f, 0.75f, 0.25f};
    map_[17] = {0.75f, 0.25f, 0.75f, 0.75f};
    map_[18] = {0.75f, 0.75f, 0.25f, 0.75f};
    map_[19] = {0.25f, 0.75f, 0.25f, 0.25f};
    // clang-format on
  }
  [[nodiscard]] const std::vector<std::array<float, 4>> &data() const { return map_; }

private:
  std::vector<std::array<float, 4>> map_{};
};

class Position {
public:
  Position(float rotation_speed, float move_speed)
      : sdl_keys_{SDL_GetKeyboardState(nullptr)}, rotation_speed_{rotation_speed}, move_speed_{move_speed} {}
  void animate(float time_elapsed) {
    if ((sdl_keys_[SDL_SCANCODE_UP] || sdl_keys_[SDL_SCANCODE_DOWN]) &&
        !(sdl_keys_[SDL_SCANCODE_UP] && sdl_keys_[SDL_SCANCODE_DOWN])) {
      const auto speed =
          (sdl_keys_[SDL_SCANCODE_UP] ? move_speed_ : 0.0f) + (sdl_keys_[SDL_SCANCODE_DOWN] ? -move_speed_ : 0.0f);
      const auto translate_x = time_elapsed * speed * std::cos(orientation());
      const auto translate_y = time_elapsed * speed * std::sin(orientation());
      x_ += translate_x;
      y_ += translate_y;
      look_x_ += translate_x;
      look_y_ += translate_y;
    }

    if ((sdl_keys_[SDL_SCANCODE_LEFT] || sdl_keys_[SDL_SCANCODE_RIGHT]) &&
        !(sdl_keys_[SDL_SCANCODE_LEFT] && sdl_keys_[SDL_SCANCODE_RIGHT])) {
      const auto orientation_diff = (sdl_keys_[SDL_SCANCODE_LEFT] ? -time_elapsed : time_elapsed) * rotation_speed_;

      const auto cos = std::cosf(orientation_diff);
      const auto sin = std::sinf(orientation_diff);
      const auto new_look_x = (look_x_ - x_) * cos - (look_y_ - y_) * sin;
      const auto new_look_y = (look_x_ - x_) * sin + (look_y_ - y_) * cos;
      look_x_ = new_look_x + x_;
      look_y_ = new_look_y + y_;

      orientation_ += orientation_diff;
      if (orientation_ < 0.0f) {
        orientation_ += std::numbers::pi * 2.0f;
      } else if (orientation_ > std::numbers::pi * 2.0f) {
        orientation_ -= std::numbers::pi * 2.0f;
      }
    }
  }
  [[nodiscard]] float orientation() const { return orientation_; }
  [[nodiscard]] float x() const { return x_; }
  [[nodiscard]] float y() const { return y_; }
  [[nodiscard]] float look_x() const { return look_x_; }
  [[nodiscard]] float look_y() const { return look_y_; }

private:
  const Uint8 *const sdl_keys_{nullptr};
  float rotation_speed_{};
  float move_speed_{};

  float orientation_{};
  float x_{};
  float y_{};
  float look_x_{4.0f};
  float look_y_{};
};

class ViewGenerator {
public:
  ViewGenerator(std::shared_ptr<const Map> map, std::shared_ptr<const Position> position, float fov,
                std::uint16_t lines)
      : map_{map}, position_{position} {
    if (lines % 2) {
      rays_.emplace_back(0.0f);
      lines--;
    }
    const float fov_step = fov * (std::numbers::pi / 180.0f) / lines;
    auto angle = fov_step;
    while (lines) {
      rays_.emplace_back(angle);
      rays_.emplace_back(-angle);
      angle += fov_step;
      lines -= 2;
    }
    std::sort(rays_.begin(), rays_.end());
    lines_.resize(rays_.size(), 0.0f);
  }
  void generate() {
    const auto x1 = position_->x();
    const auto y1 = position_->y();
    const auto x0 = position_->look_x() - x1;
    const auto y0 = position_->look_y() - y1;
    const auto view_length =
        std::sqrt(std::pow(position_->look_x() - x1, 2.0f) + std::pow(position_->look_y() - y1, 2.0f));

    for (std::size_t i{0u}; i < rays_.size(); i++) {
      const auto cos = std::cosf(rays_[i]);
      const auto sin = std::sinf(rays_[i]);
      const auto x2 = (x0 * cos - y0 * sin) + x1;
      const auto y2 = (x0 * sin + y0 * cos) + y1;

      lines_[i] = std::numeric_limits<float>::max();

      for (const auto &line : map_->data()) {
        if (tools::math::do_intersect(line[0], line[1], line[2], line[3], x1, y1, x2, y2)) {
          auto [result, x_intersect, y_intersect] =
              tools::math::intersection_point(line[0], line[1], line[2], line[3], x1, y1, x2, y2);
          if (result) {
            lines_[i] =
                std::min(lines_[i], std::sqrt(std::pow(x_intersect - x1, 2.0f) + std::pow(y_intersect - y1, 2.0f)));
          }
        }
      }

      if (lines_[i] == std::numeric_limits<float>::max()) {
        lines_[i] = 0.0f;
      } else {
        lines_[i] = (view_length - lines_[i]) / view_length;
      }
    }
  }

  const std::vector<float> &lines() const { return lines_; };

private:
  const std::shared_ptr<const Map> map_{};
  const std::shared_ptr<const Position> position_{};

  std::vector<float> rays_{};
  std::vector<float> lines_{};
};

class Renderer {
public:
  Renderer(tools::sdl::SDLSystem &sdl_sys) : window_{sdl_sys.window()}, renderer_{sdl_sys.renderer()} {
    if (!wnd())
      throw std::runtime_error{"given window is nullptr"};
    if (!r())
      throw std::runtime_error{"given renderer is nullptr"};
  }
  virtual void animate(float time_elapsed) {}
  virtual void redraw() = 0;
  void rescale() {
    int wnd_width{0};
    int wnd_height{0};
    SDL_GetWindowSize(wnd(), &wnd_width, &wnd_height);
    rescale(wnd_width, wnd_height);
  }

protected:
  [[nodiscard]] SDL_Window *wnd() const { return window_; }
  [[nodiscard]] SDL_Renderer *r() const { return renderer_; }
  virtual void rescale(int new_width, int new_height) = 0;

private:
  SDL_Window *const window_{nullptr};
  SDL_Renderer *const renderer_{nullptr};
};

class ViewRenderer : public Renderer {
public:
  ViewRenderer(tools::sdl::SDLSystem &sdl_sys, std::shared_ptr<const ViewGenerator> view_generator, std::uint16_t width,
               std::uint16_t height)
      : Renderer(sdl_sys), view_generator_{view_generator} {
    if (width == 0 || height == 0)
      throw std::runtime_error{"given width or height is invalid"};

    buffer_.resize(width, std::vector<SDL_Rect>(height, SDL_Rect{0, 0, 0, 0}));
    Renderer::rescale();
  }
  void redraw() override {
    for (std::size_t w_it{0u}; w_it < buffer_.size(); w_it++) {
      const auto line = view_generator_->lines()[w_it];
      auto wall_height = std::min(buffer_[w_it].size(), static_cast<std::size_t>(buffer_[w_it].size() * line + 0.5f));
      auto no_wall = buffer_[w_it].size() - wall_height;
      if (no_wall % 2) {
        wall_height++;
        no_wall--;
      }
      no_wall /= 2;

      // ceiling
      SDL_SetRenderDrawColor(r(), 71, 71, 71, 255);
      for (std::size_t h_it{0u}; h_it < no_wall; h_it++) {
        SDL_RenderFillRect(r(), &buffer_[w_it][h_it]);
      }
      // wall
      SDL_SetRenderDrawColor(r(), static_cast<Uint8>(65.0f * line + 0.5f), static_cast<Uint8>(80.0f * line + 0.5f),
                             120 + static_cast<Uint8>(100.0f * line + 0.5f), 255);
      for (std::size_t h_it{no_wall}; h_it < no_wall + wall_height; h_it++) {
        SDL_RenderFillRect(r(), &buffer_[w_it][h_it]);
      }
      // floor
      SDL_SetRenderDrawColor(r(), 120, 120, 120, 255);
      for (std::size_t h_it{no_wall + wall_height}; h_it < buffer_[w_it].size(); h_it++) {
        SDL_RenderFillRect(r(), &buffer_[w_it][h_it]);
      }
    }
  }

protected:
  void rescale(int new_width, int new_height) override {
    float w_part = new_width / static_cast<float>(buffer_.size());
    float h_part = new_height / static_cast<float>(buffer_[0].size());

    for (std::size_t w_it{0u}; w_it < buffer_.size(); w_it++) {
      int x = static_cast<int>(w_part * w_it);
      int w = static_cast<int>(w_part + 0.5f);
      for (std::size_t h_it{0u}; h_it < buffer_[w_it].size(); h_it++) {
        buffer_[w_it][h_it] = SDL_Rect{x, static_cast<int>(h_part * h_it), w, static_cast<int>(h_part + 0.5f)};
      }
    }
  }

private:
  const std::shared_ptr<const ViewGenerator> view_generator_{};

  std::vector<std::vector<SDL_Rect>> buffer_{};
};

class MapRenderer : public Renderer {
public:
  MapRenderer(tools::sdl::SDLSystem &sdl_sys, std::shared_ptr<const Map> map, std::shared_ptr<const Position> position)
      : Renderer(sdl_sys), map_{map}, position_{position} {
    Renderer::rescale();
  }
  void redraw() override {
    if (player_oriented_) {
      const auto cos = std::cosf(-position_->orientation());
      const auto sin = std::sinf(-position_->orientation());

      { // player
        const auto x = position_->look_x() - position_->x();
        const auto y = position_->look_y() - position_->y();
        auto x2 = x * cos - y * sin;
        x2 = x2 * scale_x_ + translate_x_;
        auto y2 = x * sin + y * cos;
        y2 = y2 * scale_y_ + translate_y_;

        SDL_SetRenderDrawColor(r(), 32, 32, 32, 255);
        SDL_RenderDrawLineF(r(), translate_x_, translate_y_, x2, y2);
      }

      { // map
        for (const auto &line : map_->data()) {
          auto x1 = (line[0] - position_->x()) * cos - (line[1] - position_->y()) * sin;
          x1 = x1 * scale_x_ + translate_x_;
          auto y1 = (line[0] - position_->x()) * sin + (line[1] - position_->y()) * cos;
          y1 = y1 * scale_y_ + translate_y_;
          auto x2 = (line[2] - position_->x()) * cos - (line[3] - position_->y()) * sin;
          x2 = x2 * scale_x_ + translate_x_;
          auto y2 = (line[2] - position_->x()) * sin + (line[3] - position_->y()) * cos;
          y2 = y2 * scale_y_ + translate_y_;

          const auto do_intersect = tools::math::do_intersect(line[0], line[1], line[2], line[3], position_->x(),
                                                              position_->y(), position_->look_x(), position_->look_y());
          if (do_intersect)
            SDL_SetRenderDrawColor(r(), 255, 0, 0, 255);
          else
            SDL_SetRenderDrawColor(r(), 64, 255, 64, 255);
          SDL_RenderDrawLineF(r(), x1, y1, x2, y2);

          if (do_intersect) { // intersection point
            auto [result, x_intersect, y_intersect] =
                tools::math::intersection_point(line[0], line[1], line[2], line[3], position_->x(), position_->y(),
                                                position_->look_x(), position_->look_y());
            if (result) {
              auto rot_x_intersect = (x_intersect - position_->x()) * cos - (y_intersect - position_->y()) * sin;
              rot_x_intersect = rot_x_intersect * scale_x_ + translate_x_;
              auto rot_y_intersect = (x_intersect - position_->x()) * sin + (y_intersect - position_->y()) * cos;
              rot_y_intersect = rot_y_intersect * scale_y_ + translate_y_;

              SDL_SetRenderDrawColor(r(), 0, 255, 0, 255);
              SDL_RenderDrawPointF(r(), rot_x_intersect, rot_y_intersect);
            }
          }
        }
      }
    } else {
      { // player
        auto posx = position_->x() * scale_x_ + translate_x_;
        auto posy = position_->y() * scale_y_ + translate_y_;
        auto lookx = position_->look_x() * scale_x_ + translate_x_;
        auto looky = position_->look_y() * scale_y_ + translate_y_;

        SDL_SetRenderDrawColor(r(), 32, 32, 32, 255);
        SDL_RenderDrawLineF(r(), posx, posy, lookx, looky);
      }

      const auto cos = std::cosf(position_->orientation());
      const auto sin = std::sinf(position_->orientation());

      { // map
        for (const auto &line : map_->data()) {
          auto x1 = line[0] * scale_x_ + translate_x_;
          auto y1 = line[1] * scale_y_ + translate_y_;
          auto x2 = line[2] * scale_x_ + translate_x_;
          auto y2 = line[3] * scale_y_ + translate_y_;

          const auto do_intersect = tools::math::do_intersect(line[0], line[1], line[2], line[3], position_->x(),
                                                              position_->y(), position_->look_x(), position_->look_y());
          if (do_intersect)
            SDL_SetRenderDrawColor(r(), 255, 0, 0, 255);
          else
            SDL_SetRenderDrawColor(r(), 64, 255, 64, 255);
          SDL_RenderDrawLineF(r(), x1, y1, x2, y2);

          if (do_intersect) { // intersection point
            auto [result, x_intersect, y_intersect] =
                tools::math::intersection_point(line[0], line[1], line[2], line[3], position_->x(), position_->y(),
                                                position_->look_x(), position_->look_y());
            if (result) {
              x_intersect = x_intersect * scale_x_ + translate_x_;
              y_intersect = y_intersect * scale_y_ + translate_y_;

              SDL_SetRenderDrawColor(r(), 0, 255, 0, 255);
              SDL_RenderDrawPointF(r(), x_intersect, y_intersect);
            }
          }
        }
      }
    }
  }

protected:
  void rescale(int new_width, int new_height) override {
    float scale_factor = 0.25f;
    scale_x_ = static_cast<float>(new_width) * scale_factor;
    scale_y_ = static_cast<float>(new_height) * scale_factor;
    translate_x_ = static_cast<float>(new_width) / 2.0f;
    translate_y_ = static_cast<float>(new_height) / 2.0f;
  }

private:
  const std::shared_ptr<const Map> map_{};
  const std::shared_ptr<const Position> position_{};

  bool player_oriented_{false};
  float scale_x_{};
  float scale_y_{};
  float translate_x_{};
  float translate_y_{};
};

class Animation {
public:
  Animation(std::uint16_t fps) : thread_{tick, fps, std::ref(should_break_)} {}
  ~Animation() {
    should_break_ = true;
    thread_.join();
  }

private:
  static void tick(std::uint16_t fps, std::atomic_bool &should_break) {
    const auto sleep_duration = std::chrono::milliseconds{1000} / fps;
    SDL_Event animation_event;
    animation_event.type = SDL_USEREVENT;
    animation_event.user.code = 0;
    animation_event.user.data1 = nullptr;
    animation_event.user.data2 = nullptr;
    while (!should_break) {
      std::this_thread::sleep_for(sleep_duration);
      if (!SDL_PushEvent(&animation_event))
        throw std::runtime_error{"animation event push failed"};
    }
  }
  std::atomic_bool should_break_{false};
  std::thread thread_{};
};
} // namespace wolf

int main(int argc, char *argv[]) {
  auto sdl_sys = tools::sdl::SDLSystem{
      SDL_INIT_EVERYTHING,  "Wolf", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 1024, 0, -1,
      SDL_RENDERER_SOFTWARE};

  auto map = std::make_shared<wolf::Map>();
  auto position = std::make_shared<wolf::Position>(0.5f, 0.5f);
  auto view_generator = std::make_shared<wolf::ViewGenerator>(map, position, 45.0f, 64);
  std::unique_ptr<wolf::Renderer> view_renderer = std::make_unique<wolf::ViewRenderer>(sdl_sys, view_generator, 64, 64);
  std::unique_ptr<wolf::Renderer> map_renderer = std::make_unique<wolf::MapRenderer>(sdl_sys, map, position);
  auto last_timestamp = SDL_GetTicks();
  auto anim = wolf::Animation(30u);

  auto quit = false;
  while (!quit) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
        quit = true;
        break;
      case SDL_WINDOWEVENT:
        switch (event.window.event) {
        case SDL_WINDOWEVENT_SIZE_CHANGED:
          view_renderer->rescale();
          map_renderer->rescale();
          break;
        case SDL_WINDOWEVENT:
          quit = true;
          break;
        default:
          break;
        }
        break;
      case SDL_USEREVENT:
        SDL_PumpEvents();
        {
          const auto time_elapsed_ms = event.user.timestamp - last_timestamp;
          last_timestamp = event.user.timestamp;
          const auto time_elapsed_s = time_elapsed_ms / 1000.0f;

          position->animate(time_elapsed_s);
        }

        view_generator->generate();

        SDL_SetRenderDrawColor(sdl_sys.renderer(), 0, 0, 0, 255);
        SDL_RenderClear(sdl_sys.renderer());
        view_renderer->redraw();
        map_renderer->redraw();
        SDL_RenderPresent(sdl_sys.renderer());
        break;
      default:
        break;
      }
    }
  }

  return 0;
}
