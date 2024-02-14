#pragma once

#include "decoder.hpp"

#include "common/common.hpp"

#include "tools/sdl/sdl_animation.hpp"
#include "tools/sdl/sdl_system.hpp"

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

private:
  std::shared_ptr<Decoder> decoder_{};
  VideoStreamInfo video_stream_info_{};
  std::unique_ptr<tools::sdl::SDLSystem> sdl_sys_{};
  std::unique_ptr<tools::sdl::SDLAnimation> animation_{};
  SDL_Surface *screen_surface_{};
  SDL_Surface *frame_surface_{};

  std::thread player_thread_{};
  std::atomic_bool quit_{false};

  void video_stream_info_callback(const VideoStreamInfo &video_stream_info);
  void player_procedure();
  void init_player();
};
} // namespace streaming
