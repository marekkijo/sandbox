# CMakeLists.txt for GP Library
#
# This CMake file is intended to create a General Purpose (GP) library, which
# contains support for various functionalities such as math operators, common
# structures used across different projects, helper functions for string
# operators, and headers for easy inclusion. It also provides higher-level
# support for projects using SDL (Simple DirectMedia Layer) and/or GLFW
# (Graphics Library Framework), if available.
#
# Please ensure that your project includes the necessary dependencies for SDL
# and GLFW.

file(GLOB_RECURSE FFMPEG_FILES "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/*")
file(GLOB_RECURSE GL_FILES "${CMAKE_CURRENT_SOURCE_DIR}/gl/*")
file(GLOB_RECURSE GLFW_FILES "${CMAKE_CURRENT_SOURCE_DIR}/glfw/*")
file(GLOB_RECURSE JSON_FILES "${CMAKE_CURRENT_SOURCE_DIR}/json/*")
file(GLOB_RECURSE MATH_FILES "${CMAKE_CURRENT_SOURCE_DIR}/math/*")
file(GLOB_RECURSE MISC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/misc/*")
file(GLOB_RECURSE SDL_FILES "${CMAKE_CURRENT_SOURCE_DIR}/sdl/*")
file(GLOB_RECURSE UTILS_FILES "${CMAKE_CURRENT_SOURCE_DIR}/utils/*")

if(NOT DEFINED EMSCRIPTEN)
  set(SRC_FILES ${SRC_FILES} ${FFMPEG_FILES})
  set(SRC_FILES ${SRC_FILES} ${GLFW_FILES})
  set(SRC_FILES ${SRC_FILES} ${GL_FILES})
endif()

set(SRC_FILES ${SRC_FILES} ${JSON_FILES})
set(SRC_FILES ${SRC_FILES} ${MATH_FILES})
set(SRC_FILES ${SRC_FILES} ${MISC_FILES})
set(SRC_FILES ${SRC_FILES} ${SDL_FILES})
set(SRC_FILES ${SRC_FILES} ${UTILS_FILES})

add_library(gp STATIC ${SRC_FILES})

target_compile_features(gp PRIVATE cxx_std_23)

target_include_directories(gp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_include_directories(gp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(gp PRIVATE glm::glm)
target_link_libraries(gp PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(gp PRIVATE SDL2::SDL2)
target_link_libraries(
  gp
  PRIVATE
    $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>
)

if(NOT DEFINED EMSCRIPTEN)
  target_include_directories(gp PRIVATE ${FFMPEG_INCLUDE_DIRS})
  target_link_directories(gp PRIVATE ${FFMPEG_LIBRARY_DIRS})
  target_link_libraries(gp PRIVATE ${FFMPEG_LIBRARIES})
  target_link_libraries(gp PRIVATE glad::glad)
  target_link_libraries(gp PRIVATE glfw)
endif()

if(DEFINED EMSCRIPTEN)
  target_compile_options(gp PRIVATE ${EMSCRIPTEN_COMPILE_OPTIONS})
  target_link_options(gp PRIVATE ${EMSCRIPTEN_LINK_OPTIONS})
endif()

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRC_FILES})
