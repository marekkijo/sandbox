cmake_minimum_required(VERSION 3.10)

project(sandbox)

set(CMAKE_CXX_STANDARD 20)

find_package(SDL2 REQUIRED)
find_package(Boost COMPONENTS program_options REQUIRED)
if(POLICY CMP0072)
    cmake_policy(SET CMP0072 NEW)
endif()
find_package(OpenGL REQUIRED)
# FFmpeg
find_path(AVCodec_INCLUDE_DIR libavcodec/avcodec.h)
find_library(AVCodec_LIBRARY avcodec)
find_path(AVFormat_INCLUDE_DIR libavformat/avformat.h)
find_library(AVFormat_LIBRARY avformat)
find_path(AVUtil_INCLUDE_DIR libavutil/avutil.h)
find_library(AVUtil_LIBRARY avutil)

add_subdirectory(projects)
add_subdirectory(tools)
