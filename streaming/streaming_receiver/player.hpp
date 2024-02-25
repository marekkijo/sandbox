#pragma once

#include "streaming_common/decoder.hpp"

#include <gp/misc/event.hpp>
#include <gp/sdl/animation.hpp>
#include <gp/sdl/system.hpp>

#include <memory>
#include <thread>

namespace streaming {
class Player {
public:
  Player(const Player &) = delete;
  Player &operator=(const Player &) = delete;
  Player(Player &&other) noexcept = delete;
  Player &operator=(Player &&other) noexcept = delete;

  Player(std::shared_ptr<Decoder> &decoder);
  ~Player();

  void set_event_callback(std::function<void(const gp::misc::Event &event)> event_callback);

private:
  void video_stream_info_callback(const VideoStreamInfo &video_stream_info);
  void player_procedure();
  void init_player();

  std::shared_ptr<Decoder> decoder_{};

  std::function<void(const gp::misc::Event &event)> event_callback_{};

  VideoStreamInfo video_stream_info_{};
  std::unique_ptr<gp::sdl::System> sdl_sys_{};
  std::unique_ptr<gp::sdl::Animation> animation_{};
  SDL_Surface *screen_surface_{};
  SDL_Surface *frame_surface_{};

  std::thread player_thread_{};
  std::atomic_bool quit_{false};
};
} // namespace streaming
