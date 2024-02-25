#include "player.hpp"

#include "streaming_common/common.hpp"

#include <gp/sdl/misc.hpp>

#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>

#include <functional>

namespace streaming {
Player::Player(std::shared_ptr<Decoder> &decoder)
    : decoder_{decoder} {
  auto video_stream_info_function = std::function<void(const VideoStreamInfo &video_stream_info)>{
      std::bind(&Player::video_stream_info_callback, this, std::placeholders::_1)};
  decoder_->set_video_stream_info_callback(video_stream_info_function);
}

Player::~Player() {
  quit_ = true;
  if (player_thread_.joinable()) { player_thread_.join(); }
}

void Player::set_event_callback(std::function<void(const gp::misc::Event &event)> event_callback) {
  event_callback_ = event_callback;
}

void Player::video_stream_info_callback(const VideoStreamInfo &video_stream_info) {
  video_stream_info_ = video_stream_info;

  auto player_procedure = std::function<void()>{std::bind(&Player::player_procedure, this)};
  player_thread_ = std::thread(player_procedure);
}

void Player::player_procedure() {
  init_player();

  auto last_timestamp_ms = SDL_GetTicks();

  while (!quit_) {
    SDL_Event sdl_event;
    while (!quit_ && SDL_PollEvent(&sdl_event)) {
      switch (sdl_event.type) {
      case SDL_QUIT:
        quit_ = true;
        break;
      case SDL_USEREVENT:
        SDL_PumpEvents();
        {
          const auto status = decoder_->decode();
          switch (status.code) {
          case Decoder::Status::Code::OK:
            printf("Decoded frame: %i\n", status.frame_num);
            SDL_BlitSurface(frame_surface_, nullptr, screen_surface_, nullptr);
            SDL_UpdateWindowSurface(sdl_sys_->wnd());
            break;
          case Decoder::Status::Code::RETRY:
            printf("Decoding: packet sent, retrying...\n");
            continue;
          case Decoder::Status::Code::EOS:
            printf("Decoding: end of stream\n");
            quit_ = true;
            break;
          case Decoder::Status::Code::NODATA:
            printf("Decoding: no data left to decode\n");
            break;
          case Decoder::Status::Code::ERROR_:
            printf("Decoding: ERROR\n");
            quit_ = true;
            break;
          }
        }
        break;
      case SDL_MOUSEMOTION:
      case SDL_MOUSEBUTTONDOWN:
      case SDL_MOUSEBUTTONUP:
      case SDL_MOUSEWHEEL:
      case SDL_KEYDOWN:
      case SDL_KEYUP:
        if (event_callback_) {
          const auto event = gp::sdl::to_event(sdl_event);
          event_callback_(event);
        }
        break;
      default:
        break;
      }
    }
  }

  SDL_FreeSurface(frame_surface_);

  animation_.reset();
  sdl_sys_.reset();
}

void Player::init_player() {
  const auto wnd_title = std::string(RECEIVER_ID) + " (" + video_stream_info_.codec_name + ")";
  sdl_sys_ = std::make_unique<gp::sdl::System>(SDL_INIT_EVERYTHING,
                                               wnd_title.c_str(),
                                               SDL_WINDOWPOS_CENTERED,
                                               SDL_WINDOWPOS_CENTERED,
                                               video_stream_info_.width,
                                               video_stream_info_.height,
                                               0,
                                               -1,
                                               SDL_RENDERER_SOFTWARE);
  animation_ = std::make_unique<gp::sdl::Animation>(video_stream_info_.fps);

  screen_surface_ = SDL_GetWindowSurface(sdl_sys_->wnd());

  const auto pitch = CHANNELS_NUM * video_stream_info_.width;
  frame_surface_ = SDL_CreateRGBSurfaceFrom(decoder_->rgb_frame()->data(),
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
