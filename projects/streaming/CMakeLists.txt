include_directories(${PROJECT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(SOLUTION_FOLDER projects/streaming)

set(LIBDATACHANNEL_DLL "$<IF:$<CONFIG:Debug>,${libdatachannel_PACKAGE_FOLDER_DEBUG}/bin/datachannel.dll,${libdatachannel_PACKAGE_FOLDER_RELEASE}/bin/datachannel.dll>")

add_executable(opengl_streamer
  opengl_streamer/main.cpp
  opengl_streamer/encoder.cpp
  opengl_streamer/encoder.hpp
  opengl_streamer/decoder.cpp
  opengl_streamer/decoder.hpp
)

target_link_libraries(opengl_streamer tools_sdl)
target_link_libraries(opengl_streamer SDL2::SDL2)
target_link_libraries(opengl_streamer SDL2::SDL2main)
target_link_libraries(opengl_streamer Boost::program_options)
target_link_libraries(opengl_streamer opengl::opengl)
target_link_libraries(opengl_streamer ffmpeg::avcodec)
target_link_libraries(opengl_streamer ffmpeg::avformat)
target_link_libraries(opengl_streamer ffmpeg::avutil)
target_link_libraries(opengl_streamer ffmpeg::swscale)
target_link_libraries(opengl_streamer glm::glm)
target_link_libraries(opengl_streamer GLEW::GLEW)

set_target_properties(opengl_streamer PROPERTIES
  FOLDER ${SOLUTION_FOLDER}
)

add_library(common INTERFACE
  common/common.hpp
  common/ffmpeg.hpp
)

target_link_libraries(common INTERFACE ffmpeg::avcodec)

set_target_properties(common PROPERTIES
  FOLDER ${SOLUTION_FOLDER}
)

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
target_link_libraries(signaling_server libdatachannel::libdatachannel)
target_link_libraries(signaling_server nlohmann_json::nlohmann_json)

add_custom_command(TARGET signaling_server PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
  ${LIBDATACHANNEL_DLL} $<TARGET_FILE_DIR:signaling_server>/
)

set_target_properties(signaling_server PROPERTIES
  FOLDER ${SOLUTION_FOLDER}
  ${LIBDATACHANNEL_DLL} $<TARGET_FILE_DIR:signaling_server>
)

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
target_link_libraries(streamer tools_common)
target_link_libraries(streamer tools_sdl)
target_link_libraries(streamer tools_utils)
target_link_libraries(streamer SDL2::SDL2)
target_link_libraries(streamer Boost::program_options)
target_link_libraries(streamer openssl::openssl)
target_link_libraries(streamer ffmpeg::avcodec)
target_link_libraries(streamer ffmpeg::avutil)
target_link_libraries(streamer ffmpeg::swscale)
target_link_libraries(streamer opengl::opengl)
target_link_libraries(streamer glm::glm)
target_link_libraries(streamer GLEW::GLEW)
target_link_libraries(streamer libdatachannel::libdatachannel)
target_link_libraries(streamer nlohmann_json::nlohmann_json)

add_custom_command(TARGET streamer PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
  ${LIBDATACHANNEL_DLL} $<TARGET_FILE_DIR:streamer>/
)

add_custom_command(TARGET streamer PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_CURRENT_SOURCE_DIR}/streamer/shaders $<TARGET_FILE_DIR:streamer>/shaders
)

set_target_properties(streamer PROPERTIES
  FOLDER ${SOLUTION_FOLDER}
  VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:streamer>
)

add_executable(receiver
  receiver/main.cpp
  receiver/decoder.cpp
  receiver/decoder.hpp
  receiver/player.cpp
  receiver/player.hpp
  receiver/receiver.cpp
  receiver/receiver.hpp
)

target_link_libraries(receiver common)
target_link_libraries(receiver tools_common)
target_link_libraries(receiver tools_sdl)
target_link_libraries(receiver tools_utils)
target_link_libraries(receiver Boost::program_options)
target_link_libraries(receiver openssl::openssl)
target_link_libraries(receiver ffmpeg::avcodec)
target_link_libraries(receiver ffmpeg::avutil)
target_link_libraries(receiver ffmpeg::swscale)
target_link_libraries(receiver libdatachannel::libdatachannel)
target_link_libraries(receiver nlohmann_json::nlohmann_json)

add_custom_command(TARGET receiver PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
  ${LIBDATACHANNEL_DLL} $<TARGET_FILE_DIR:receiver>/
)

set_target_properties(receiver PROPERTIES
  FOLDER ${SOLUTION_FOLDER}
  VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:receiver>
)
