cmake_minimum_required(VERSION 3.15)

project(sandbox CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

find_package(Boost COMPONENTS program_options REQUIRED)
find_package(ffmpeg COMPONENTS avcodec avformat avutil swscale REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(LibDataChannel REQUIRED)
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(assimp REQUIRED)
find_package(imgui REQUIRED)

add_subdirectory(tools)
add_subdirectory(projects)
