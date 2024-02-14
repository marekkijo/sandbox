#include "player.hpp"

#include <SDL2/SDL.h>

#include <functional>

namespace streaming {
Player::Player(std::shared_ptr<Decoder> &decoder) : decoder_{decoder} {
  auto video_stream_info_function = std::function<void(const VideoStreamInfo &video_stream_info)>{
      std::bind(&Player::video_stream_info_callback, this, std::placeholders::_1)};
  decoder_->set_video_stream_info_callback(video_stream_info_function);
}

Player::~Player() {
  quit_ = true;
  if (player_thread_.joinable()) { player_thread_.join(); }
}

void Player::video_stream_info_callback(const VideoStreamInfo &video_stream_info) {
  video_stream_info_ = video_stream_info;

  auto player_procedure = std::function<void()>{std::bind(&Player::player_procedure, this)};
  player_thread_        = std::thread(player_procedure);
}

void Player::player_procedure() {
  init_player();

  auto last_timestamp_ms = SDL_GetTicks();

  while (!quit_) {
    SDL_Event event;
    while (!quit_ && SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT: quit_ = true; break;
      case SDL_USEREVENT:
        SDL_PumpEvents();
        if (decoder_->prepare_frame()) {
          SDL_BlitSurface(frame_surface_, nullptr, screen_surface_, nullptr);
          SDL_UpdateWindowSurface(sdl_sys_->wnd());
        }
        break;
      default: break;
      }
    }
  }

  SDL_FreeSurface(frame_surface_);

  animation_.reset();
  sdl_sys_.reset();
}

void Player::init_player() {
  sdl_sys_   = std::make_unique<tools::sdl::SDLSystem>(SDL_INIT_EVERYTHING,
                                                     RECEIVER_ID,
                                                     SDL_WINDOWPOS_CENTERED,
                                                     SDL_WINDOWPOS_CENTERED,
                                                     video_stream_info_.width,
                                                     video_stream_info_.height,
                                                     0,
                                                     -1,
                                                     SDL_RENDERER_SOFTWARE);
  animation_ = std::make_unique<tools::sdl::SDLAnimation>(video_stream_info_.fps);

  screen_surface_ = SDL_GetWindowSurface(sdl_sys_->wnd());

  const auto pitch = CHANNELS_NUM * video_stream_info_.width;
  frame_surface_   = SDL_CreateRGBSurfaceFrom(decoder_->rgb_frame()->data(),
                                            video_stream_info_.width,
                                            video_stream_info_.height,
                                            8 * CHANNELS_NUM,
                                            pitch,
                                            0x000000FF,
                                            0x0000FF00,
                                            0x00FF0000,
                                            0x00000000);
}

} // namespace streaming
