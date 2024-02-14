#include "encoder.hpp"

#include "tools/sdl/sdl_animation.hpp"
#include "tools/sdl/sdl_system.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>

#include <boost/program_options.hpp>

#include <cstdint>
#include <iostream>
#include <memory>

struct ProgramSetup {
  bool exit{};

  int          width{};
  int          height{};
  unsigned int fps{};
  unsigned int seconds{};
};

ProgramSetup process_args(const int argc, const char *const argv[]) {
  boost::program_options::options_description desc("Wolf options");
  desc.add_options()("help", "This help message");
  desc.add_options()("width", boost::program_options::value<int>()->default_value(512), "Width of the frame buffer");
  desc.add_options()("height", boost::program_options::value<int>()->default_value(384), "Height of the frame buffer");
  desc.add_options()("fps",
                     boost::program_options::value<unsigned int>()->default_value(30u),
                     "Number of frames per second");
  desc.add_options()("seconds",
                     boost::program_options::value<unsigned int>()->default_value(10u),
                     "Length of the stream in seconds");

  boost::program_options::variables_map vm;
  boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
  boost::program_options::notify(vm);

  if (vm.count("help")) {
    desc.print(std::cout);
    return {true};
  }

  return {false,
          vm["width"].as<int>(),
          vm["height"].as<int>(),
          vm["fps"].as<unsigned int>(),
          vm["seconds"].as<unsigned int>()};
}

int main(int argc, char *argv[]) {
  const auto program_setup = process_args(argc, argv);
  if (program_setup.exit) { return 1; }

  const auto wnd_title = "OpenGL Streamer";

  auto sdl_sys    = tools::sdl::SDLSystem{SDL_INIT_EVERYTHING,
                                       wnd_title,
                                       SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED,
                                       program_setup.width,
                                       program_setup.height,
                                       SDL_WINDOW_OPENGL,
                                       -1,
                                       SDL_RENDERER_SOFTWARE};
  auto gl_context = SDL_GL_CreateContext(sdl_sys.wnd());

  auto last_timestamp_ms = SDL_GetTicks();
  auto animation         = tools::sdl::SDLAnimation(program_setup.fps);
  auto quit              = false;

  glReadBuffer(GL_BACK);

  auto         gl_frame = std::make_shared<std::vector<GLubyte>>(program_setup.width * program_setup.height * 4);
  auto         encoder  = streaming::Encoder(program_setup.width, program_setup.height, gl_frame, program_setup.fps);
  unsigned int frames   = program_setup.seconds * program_setup.fps + 1;
  while (!quit) {
    SDL_Event event;
    while (!quit && SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT: quit = true; break;
      case SDL_USEREVENT:
        SDL_PumpEvents();
        {
          const auto time_elapsed_ms = event.user.timestamp - last_timestamp_ms;

          // renderer->animate(time_elapsed_ms);

          last_timestamp_ms = event.user.timestamp;
        }
        glClearColor(1.0f, 0.0f, 1.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glFlush();
        glReadPixels(0, 0, program_setup.width, program_setup.height, GL_RGBA, GL_UNSIGNED_BYTE, gl_frame->data());
        encoder.encode_frame();
        frames--;
        if (frames == 0) { quit = true; }
        break;
      default: break;
      }
    }
  }

  return 0;
}
