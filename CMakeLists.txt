cmake_minimum_required(VERSION 3.20)

project(sandbox CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

find_package(assimp CONFIG REQUIRED)
find_package(
  Boost
  COMPONENTS program_options
  REQUIRED)
find_package(
  ffmpeg
  COMPONENTS avcodec avformat avutil swscale
  REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(libdatachannel CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)
find_path(CGLTF_INCLUDE_DIRS "cgltf.h")

set(SOURCE_GROUP_LABEL src)

add_subdirectory(ai)
add_subdirectory(gp)
add_subdirectory(loaders)
add_subdirectory(streaming)
add_subdirectory(wolf)
