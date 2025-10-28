# CMakeLists.txt for GP Library
#
# This CMake file is intended to create a General Purpose (GP) library, which
# contains support for various functionalities such as math operators, common
# structures used across different projects, helper functions for string
# operators, and headers for easy inclusion. It also provides higher-level
# support for projects using SDL (Simple DirectMedia Layer) and OpenGL.
#
# Please ensure that your project includes the necessary dependencies for SDL
# and OpenGL.

set(SOLUTION_FOLDER gp)

file(GLOB_RECURSE FFMPEG_FILES "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/*")
file(GLOB_RECURSE GL_FILES "${CMAKE_CURRENT_SOURCE_DIR}/gl/*")
file(GLOB_RECURSE JSON_FILES "${CMAKE_CURRENT_SOURCE_DIR}/json/*")
file(GLOB_RECURSE MATH_FILES "${CMAKE_CURRENT_SOURCE_DIR}/math/*")
file(GLOB_RECURSE MISC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/misc/*")
file(GLOB_RECURSE SDL_FILES "${CMAKE_CURRENT_SOURCE_DIR}/sdl/*")
file(GLOB_RECURSE UTILS_FILES "${CMAKE_CURRENT_SOURCE_DIR}/utils/*")

if(NOT EMSCRIPTEN)
  set(SRC_FILES ${SRC_FILES} ${FFMPEG_FILES})
endif()

set(SRC_FILES ${SRC_FILES} ${GL_FILES})
set(SRC_FILES ${SRC_FILES} ${JSON_FILES})
set(SRC_FILES ${SRC_FILES} ${MATH_FILES})
set(SRC_FILES ${SRC_FILES} ${MISC_FILES})
set(SRC_FILES ${SRC_FILES} ${SDL_FILES})
set(SRC_FILES ${SRC_FILES} ${UTILS_FILES})

add_library(gp STATIC ${SRC_FILES})

target_compile_features(gp PRIVATE cxx_std_23)

target_include_directories(gp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_include_directories(gp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(gp PUBLIC glm::glm)
target_link_libraries(gp PUBLIC nlohmann_json::nlohmann_json)
target_link_libraries(gp PUBLIC SDL2::SDL2)
target_link_libraries(gp PUBLIC SDL2::SDL2main)
target_link_libraries(
  gp
  PUBLIC
    $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>
)

if(NOT EMSCRIPTEN)
  target_include_directories(gp PUBLIC ${FFMPEG_INCLUDE_DIRS})
  target_link_directories(gp PUBLIC ${FFMPEG_LIBRARY_DIRS})
  target_link_libraries(gp PUBLIC ${FFMPEG_LIBRARIES})
  target_link_libraries(gp PUBLIC glad::glad)
endif()

if(EMSCRIPTEN)
  target_compile_options(gp PRIVATE ${PRESET_COMPILE_OPTIONS})
  target_link_options(gp PRIVATE ${PRESET_LINK_OPTIONS})
endif()

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRC_FILES})

if(NOT EMSCRIPTEN)
  add_subdirectory(${TESTS_DIRECTORY})
endif()
