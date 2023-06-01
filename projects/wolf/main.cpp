#include "map_renderer.hpp"
#include "player_state.hpp"
#include "raw_map.hpp"
#include "raw_map_from_ascii.hpp"
#include "raw_map_from_wolf.hpp"
#include "vector_map.hpp"
#include "wolf_renderer.hpp"

#include "tools/sdl/sdl_animation.hpp"
#include "tools/sdl/sdl_system.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>

#include <memory>

int main(int argc, char *argv[]) {
  auto wnd_title         = "Wolf";
  auto asciimap_filename = "projects/wolf/data/map3.map";
  auto maphead_filename  = "projects/wolf/data/MAPHEAD.WL6";
  auto gamemaps_filename = "projects/wolf/data/GAMEMAPS.WL6";
  auto pix_width         = 1024;
  auto pix_height        = 1024;
  auto rays_number       = 128u;

  auto sdl_sys = tools::sdl::SDLSystem{SDL_INIT_EVERYTHING,
                                       wnd_title,
                                       SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED,
                                       pix_width,
                                       pix_height,
                                       SDL_WINDOW_RESIZABLE,
                                       -1,
                                       SDL_RENDERER_SOFTWARE};

  auto raw_map_from_wolf = wolf::RawMapFromWolf{maphead_filename, gamemaps_filename};
  auto raw_map           = std::shared_ptr(std::move(raw_map_from_wolf.create_map(0u)));
  // auto raw_map_from_ascii = wolf::RawMapFromAscii{asciimap_filename};
  // auto raw_map            = std::shared_ptr(std::move(raw_map_from_ascii.create_map()));

  auto vector_map   = std::make_shared<wolf::VectorMap>(std::const_pointer_cast<const wolf::RawMap>(raw_map));
  auto player_state = std::make_shared<wolf::PlayerState>(std::const_pointer_cast<const wolf::RawMap>(raw_map));

  auto map_renderer = wolf::MapRenderer{sdl_sys,
                                        std::const_pointer_cast<const wolf::VectorMap>(vector_map),
                                        std::const_pointer_cast<const wolf::PlayerState>(player_state)};

  auto wolf_renderer = wolf::WolfRenderer{sdl_sys,
                                          std::const_pointer_cast<const wolf::VectorMap>(vector_map),
                                          std::const_pointer_cast<const wolf::PlayerState>(player_state),
                                          rays_number};

  auto last_timestamp_ms = SDL_GetTicks();
  auto animation         = tools::sdl::SDLAnimation(30u);

  auto quit = false;
  while (!quit) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT: quit = true; break;
      case SDL_WINDOWEVENT:
        switch (event.window.event) {
        case SDL_WINDOWEVENT_SIZE_CHANGED:
          wolf_renderer.Renderer::resize();
          map_renderer.Renderer::resize();
          break;
        default: break;
        }
        break;
      case SDL_USEREVENT:
        SDL_PumpEvents();
        {
          const auto time_elapsed_ms = event.user.timestamp - last_timestamp_ms;
          player_state->animate(time_elapsed_ms);
          last_timestamp_ms = event.user.timestamp;
        }
        SDL_SetRenderDrawColor(sdl_sys.r(), 0, 0, 0, 255);
        SDL_RenderClear(sdl_sys.r());
        wolf_renderer.redraw();
        map_renderer.redraw();
        SDL_RenderPresent(sdl_sys.r());
        break;
      default: break;
      }
    }
  }

  return 0;
}
