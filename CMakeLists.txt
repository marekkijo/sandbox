cmake_minimum_required(VERSION 3.15)

project(sandbox CXX)

find_package(Boost COMPONENTS program_options REQUIRED)
find_package(ffmpeg COMPONENTS avcodec avformat avutil swscale REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(LibDataChannel REQUIRED)
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

add_subdirectory(tools)
add_subdirectory(projects/wolf)
add_subdirectory(projects/streaming)
