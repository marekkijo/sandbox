cmake_minimum_required(VERSION 3.30)

project(sandbox CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
  set(EMSCRIPTEN TRUE)

  set(CMAKE_EXECUTABLE_SUFFIX ".html")
  set(EMSCRIPTEN_COMPILE_OPTIONS "-pthread")
  set(EMSCRIPTEN_LINK_OPTIONS "-O3" "-pthread")
endif()

find_package(assimp CONFIG REQUIRED)
find_package(
  Boost
  COMPONENTS program_options
  REQUIRED)
find_path(CGLTF_INCLUDE_DIRS "cgltf.h")
find_package(glm CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)

if(NOT DEFINED EMSCRIPTEN)
  find_package(
    ffmpeg
    COMPONENTS avcodec avformat avutil swscale
    REQUIRED)
  find_package(glad CONFIG REQUIRED)
  find_package(glfw3 CONFIG REQUIRED)
  find_package(libdatachannel CONFIG REQUIRED)
endif()

set(SOURCE_GROUP_LABEL src)

if(NOT DEFINED EMSCRIPTEN)
  add_subdirectory(streaming)
  add_subdirectory(loaders)
endif()

add_subdirectory(ai)
add_subdirectory(gp)
add_subdirectory(wolf)

# Copy compile_commands.json to the source directory if using Ninja or
# Makefiles. compile_commands.json is used by clang-tidy and other tools to
# replicate the build commands.
if(NOT DEFINED EMSCRIPTEN)
  set(CMAKE_CXX_CLANG_TIDY "clang-tidy")
  set(CMAKE_CXX_CLANG_TIDY_CONFIG_FILE "${CMAKE_SOURCE_DIR}/.clang-tidy")
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

  if(CMAKE_GENERATOR MATCHES "Ninja" OR CMAKE_GENERATOR MATCHES "Makefiles")
    set(COMPILE_COMMANDS_SRC "${CMAKE_BINARY_DIR}/compile_commands.json")
    set(COMPILE_COMMANDS_DST "${CMAKE_SOURCE_DIR}/compile_commands.json")

    add_custom_target(
      copy_compile_commands ALL
      COMMAND ${CMAKE_COMMAND} -E copy_if_different "${COMPILE_COMMANDS_SRC}"
              "${COMPILE_COMMANDS_DST}"
      DEPENDS "${COMPILE_COMMANDS_SRC}"
      COMMENT "Copying compile_commands.json to source directory")
  endif()
endif()
