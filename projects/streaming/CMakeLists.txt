include_directories(${PROJECT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(opengl_streamer
  opengl_streamer/main.cpp
  opengl_streamer/encoder.cpp
  opengl_streamer/encoder.hpp
  opengl_streamer/decoder.cpp
  opengl_streamer/decoder.hpp
)

target_link_libraries(opengl_streamer tools_sdl)
target_link_libraries(opengl_streamer SDL2)
target_link_libraries(opengl_streamer Boost::program_options)
target_link_libraries(opengl_streamer OpenGL)
target_link_libraries(opengl_streamer ${AVCodec_LIBRARY})
target_link_libraries(opengl_streamer ${AVFormat_LIBRARY})
target_link_libraries(opengl_streamer ${AVUtil_LIBRARY})
target_link_libraries(opengl_streamer ${SWScale_LIBRARY})
target_link_libraries(opengl_streamer glm::glm)
target_link_libraries(opengl_streamer GLEW::GLEW)

add_library(common INTERFACE
  common/common.hpp
)

target_link_libraries(common INTERFACE ${AVCodec_LIBRARY})

add_executable(signaling_server
  signaling_server/main.cpp
  signaling_server/client.cpp
  signaling_server/client.hpp
  signaling_server/server.cpp
  signaling_server/server.hpp
)

target_link_libraries(signaling_server common)
target_link_libraries(signaling_server tools_utils)
target_link_libraries(signaling_server Boost::program_options)
target_link_libraries(signaling_server LibDataChannel::LibDataChannel)
target_link_libraries(signaling_server nlohmann_json::nlohmann_json)

add_executable(streamer
  streamer/encoder.cpp
  streamer/encoder.hpp
  streamer/main.cpp
  streamer/renderer.cpp
  streamer/renderer.hpp
  streamer/streamer.cpp
  streamer/streamer.hpp
)

target_link_libraries(streamer common)
target_link_libraries(streamer tools_sdl)
target_link_libraries(streamer tools_utils)
target_link_libraries(streamer SDL2)
target_link_libraries(streamer Boost::program_options)
target_link_libraries(streamer ${AVCodec_LIBRARY})
target_link_libraries(streamer ${AVUtil_LIBRARY})
target_link_libraries(streamer ${SWScale_LIBRARY})
target_link_libraries(streamer OpenGL)
target_link_libraries(streamer glm::glm)
target_link_libraries(streamer GLEW::GLEW)
target_link_libraries(streamer LibDataChannel::LibDataChannel)

configure_file(streamer/shader_program.fs ${CMAKE_CURRENT_BINARY_DIR}/shader_program.fs COPYONLY)
configure_file(streamer/shader_program.vs ${CMAKE_CURRENT_BINARY_DIR}/shader_program.vs COPYONLY)

add_executable(receiver
  receiver/main.cpp
  receiver/decoder.cpp
  receiver/decoder.hpp
  receiver/player.cpp
  receiver/player.hpp
  receiver/receiver.cpp
  receiver/receiver.hpp
)

target_link_libraries(receiver tools_sdl)
target_link_libraries(receiver tools_utils)
target_link_libraries(receiver Boost::program_options)
target_link_libraries(receiver ${AVCodec_LIBRARY})
target_link_libraries(receiver ${AVUtil_LIBRARY})
target_link_libraries(receiver ${SWScale_LIBRARY})
target_link_libraries(receiver LibDataChannel::LibDataChannel)
