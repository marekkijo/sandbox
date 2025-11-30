#include "raycaster_scene.hpp"
#include "wolf_common/player_state.hpp"

namespace wolf {
RaycasterScene::RaycasterScene(std::unique_ptr<const RawMap> raw_map, const int fov_in_degrees, const int num_rays)
    : raw_map_{std::move(raw_map)}
    , player_state_{*raw_map_}
    , raycaster_{*raw_map_, player_state_, fov_in_degrees, num_rays} {}

void RaycasterScene::loop(const gp::misc::Event &event) {
  switch (event.type()) {
  case gp::misc::Event::Type::Init:
    last_timestamp_ms_ = event.timestamp();
    player_state_.set_keyboard_state(keyboard_state());
    resize(event.init().width, event.init().height);
    break;
  case gp::misc::Event::Type::Quit:
    player_state_.set_keyboard_state(nullptr);
    break;
  case gp::misc::Event::Type::Resize:
    resize(event.resize().width, event.resize().height);
    break;
  case gp::misc::Event::Type::Redraw: {
    const auto time_elapsed_ms = event.timestamp() - last_timestamp_ms_;
    player_state_.animate(time_elapsed_ms);
    raycaster_.cast_rays();
    last_timestamp_ms_ = event.timestamp();
    redraw();
  } break;
  default:
    break;
  }
}

void RaycasterScene::resize(const int width, const int height) {
  width_ = width;
  height_ = height;
}

void RaycasterScene::redraw() {
  r().set_color(24, 24, 24);
  r().clear();

  r().present();
}
} // namespace wolf
