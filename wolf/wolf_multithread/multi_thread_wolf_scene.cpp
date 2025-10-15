#include "multi_thread_wolf_scene.hpp"

#include <gp/math/math.hpp>

#include <cmath>
#include <condition_variable>
#include <mutex>
#include <numbers>

namespace wolf {
struct MultiThreadWolfScene::PrepareWallsThreadState {
  MultiThreadWolfScene &scene;

  glm::vec2 cam_1{};
  glm::vec2 cam_2{};

  std::mutex m{};
  std::condition_variable cv{};
  std::condition_variable cv_main{};
  std::vector<bool> do_work{};
  std::uint32_t done{0u};
  bool join{false};
};

class MultiThreadWolfScene::PrepareWallsThreadCallable {
public:
  PrepareWallsThreadCallable(const std::size_t id,
                             const std::size_t ray_first,
                             const std::size_t ray_one_past_last,
                             MultiThreadWolfScene::PrepareWallsThreadState &state)
      : id_{id}
      , ray_first_{ray_first}
      , ray_one_past_last_{ray_one_past_last}
      , state_{state} {}

  void operator()() {
    constexpr auto ray_length{100.0f};

    std::unique_lock lk(state_.m);

    const auto &dir = state_.scene.player_state_->dir();
    const auto &pos = state_.scene.player_state_->pos();
    const auto &vector_map = *state_.scene.vector_map_;
    const auto &cam_1 = state_.cam_1;
    const auto &cam_2 = state_.cam_2;
    auto &ray_rots = state_.scene.ray_rots_;
    auto &walls = state_.scene.walls_;
    do {
      state_.cv.wait(lk, [&]() { return state_.do_work.at(id_); });
      state_.do_work.at(id_) = false;
      if (state_.join) {
        break;
      }
      lk.unlock();

      for (auto r_it = ray_first_; r_it < ray_one_past_last_; r_it++) {
        const auto ray_cos = ray_rots.at(r_it).cos;
        const auto ray_sin = ray_rots.at(r_it).sin;
        const auto ray_x = (ray_cos * dir.x - ray_sin * dir.y) * ray_length + pos.x;
        const auto ray_y = (ray_sin * dir.x + ray_cos * dir.y) * ray_length + pos.y;

        auto v_index = std::numeric_limits<std::size_t>::max();
        auto min_dist = ray_length * 2.0f;
        auto min_x = 0.0f;
        auto min_y = 0.0f;
        for (auto v_it = std::size_t{0u}; v_it < vector_map.vectors().size(); v_it++) {
          const auto &v = vector_map.vectors().at(v_it);

          if (!gp::math::do_intersect(pos.x, pos.y, ray_x, ray_y, v.first.x, v.first.y, v.second.x, v.second.y)) {
            continue;
          }
          const auto [crossed, cross_x, cross_y] =
              gp::math::intersection_point(pos.x, pos.y, ray_x, ray_y, v.first.x, v.first.y, v.second.x, v.second.y);
          if (!crossed) {
            continue;
          }

          const auto curr_dist =
              std::sqrtf((cross_x - pos.x) * (cross_x - pos.x) + (cross_y - pos.y) * (cross_y - pos.y));
          if (curr_dist < min_dist) {
            min_dist = curr_dist;
            min_x = cross_x;
            min_y = cross_y;
            v_index = v_it;
          }
        }

        auto cam_dist = std::numeric_limits<float>::max();
        if (v_index != std::numeric_limits<std::size_t>::max()) {
          cam_dist = (cam_2.x - cam_1.x) * (cam_1.y - min_y) - (cam_1.x - min_x) * (cam_2.y - cam_1.y);
          cam_dist /= std::sqrtf((cam_2.x - cam_1.x) * (cam_2.x - cam_1.x) + (cam_2.y - cam_1.y) * (cam_2.y - cam_1.y));
          if (cam_dist < 0.0f) {
            cam_dist = 0.0f;
          }
        }

        walls.at(r_it).rect.h = 1.0f / cam_dist * state_.scene.height_;
        walls.at(r_it).rect.y = (state_.scene.height_ - walls.at(r_it).rect.h) / 2.0f;
        walls.at(r_it).color_index = v_index == std::numeric_limits<std::size_t>::max() ? 0u : v_index;

        const int num_steps = 8;
        const float step_size = 0.75f / num_steps;
        const float shadow_factor =
            1.0f - std::min(0.75f, static_cast<float>(walls.at(r_it).rect.h) / state_.scene.height_);

        walls.at(r_it).shadow_factor = static_cast<float>(static_cast<int>(shadow_factor / step_size)) * step_size;
      }

      lk.lock();
      state_.done++;
      state_.cv_main.notify_one();
    } while (true);
  }

private:
  const std::size_t id_{};
  const std::size_t ray_first_{};
  const std::size_t ray_one_past_last_{};
  MultiThreadWolfScene::PrepareWallsThreadState &state_;
};

MultiThreadWolfScene::MultiThreadWolfScene(const RawMap &raw_map,
                                           const std::uint32_t fov_in_degrees,
                                           const std::uint32_t number_of_rays,
                                           const std::uint32_t number_of_threads)
    : WolfScene{raw_map, fov_in_degrees, number_of_rays}
    , state_{std::make_unique<PrepareWallsThreadState>(*this)} {
  state_->do_work = std::vector<bool>(number_of_threads, false);

  const auto rays_per_thread = static_cast<float>(ray_rots_.size()) / number_of_threads;
  threads_.reserve(number_of_threads);
  for (auto t_it = std::size_t{0u}; t_it < number_of_threads; t_it++) {
    const auto ray_first = static_cast<std::size_t>(std::floor(t_it * rays_per_thread));

    const auto last_thread = t_it == number_of_threads - 1;
    const auto ray_one_past_last =
        last_thread ? ray_rots_.size() : static_cast<std::size_t>(std::floor((t_it + 1) * rays_per_thread));

    threads_.emplace_back(PrepareWallsThreadCallable{t_it, ray_first, ray_one_past_last, *state_});
  }
}

MultiThreadWolfScene::~MultiThreadWolfScene() {
  state_->join = true;
  std::fill(state_->do_work.begin(), state_->do_work.end(), true);
  state_->cv.notify_all();
  for (auto t_it = std::size_t{0u}; t_it < threads_.size(); t_it++) {
    threads_.at(t_it).join();
  }
}

void MultiThreadWolfScene::prepare_walls() {
  const auto cam_cos1 = std::cosf(-std::numbers::pi / 2.0f);
  const auto cam_sin1 = std::sinf(-std::numbers::pi / 2.0f);
  const auto cam_cos2 = std::cosf(std::numbers::pi / 2.0f);
  const auto cam_sin2 = std::sinf(std::numbers::pi / 2.0f);

  const auto &dir = player_state_->dir();
  const auto &pos = player_state_->pos();

  state_->cam_1 =
      glm::vec2{(cam_cos1 * dir.x - cam_sin1 * dir.y) + pos.x, (cam_sin1 * dir.x + cam_cos1 * dir.y) + pos.y};
  state_->cam_2 =
      glm::vec2{(cam_cos2 * dir.x - cam_sin2 * dir.y) + pos.x, (cam_sin2 * dir.x + cam_cos2 * dir.y) + pos.y};

  state_->done = 0u;
  std::fill(state_->do_work.begin(), state_->do_work.end(), true);
  state_->cv.notify_all();

  std::unique_lock lk(state_->m);
  state_->cv_main.wait(lk, [&]() { return state_->done == threads_.size(); });
}
} // namespace wolf
