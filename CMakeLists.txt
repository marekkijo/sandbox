cmake_minimum_required(VERSION 3.10)

project(sandbox)

set(CMAKE_CXX_STANDARD 20)

find_package(SDL2 REQUIRED)
find_package(Boost COMPONENTS program_options REQUIRED)
if(POLICY CMP0072)
    cmake_policy(SET CMP0072 NEW)
endif()
find_package(OpenGL REQUIRED)
find_library(AVCodec_LIBRARY avcodec)
find_library(AVFormat_LIBRARY avformat)
find_library(AVUtil_LIBRARY avutil)
find_library(SWScale_LIBRARY swscale)
find_package(glm REQUIRED)
find_package(GLEW REQUIRED)
find_package(LibDataChannel REQUIRED)
find_package(nlohmann_json REQUIRED)

add_subdirectory(projects)
add_subdirectory(tools)
