cmake_minimum_required(VERSION 3.15)

project(sandbox CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

find_package(assimp REQUIRED)
find_package(
  Boost
  COMPONENTS program_options
  REQUIRED)
find_package(
  ffmpeg
  COMPONENTS avcodec avformat avutil swscale
  REQUIRED)
find_package(glad REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(LibDataChannel REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(SDL2 REQUIRED)
find_package(cgltf REQUIRED)

set(SOURCE_GROUP_LABEL src)

add_subdirectory(gp)
add_subdirectory(loaders)
add_subdirectory(streaming)
add_subdirectory(wolf)
