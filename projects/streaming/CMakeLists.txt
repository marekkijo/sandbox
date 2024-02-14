add_executable(opengl_streamer
  opengl_streamer/main.cpp
  opengl_streamer/encoder.cpp
  opengl_streamer/encoder.hpp
  opengl_streamer/decoder.cpp
  opengl_streamer/decoder.hpp
)

include_directories(${PROJECT_SOURCE_DIR})
target_link_libraries(opengl_streamer tools_sdl)

target_link_libraries(opengl_streamer SDL2)
target_link_libraries(opengl_streamer Boost::program_options)
target_link_libraries(opengl_streamer OpenGL)
target_link_libraries(opengl_streamer ${AVCodec_LIBRARY})
target_link_libraries(opengl_streamer ${AVFormat_LIBRARY})
target_link_libraries(opengl_streamer ${AVUtil_LIBRARY})
target_link_libraries(opengl_streamer ${SWScale_LIBRARY})
