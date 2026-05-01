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

file(GLOB_RECURSE FFMPEG_FILES CONFIGURE_DEPENDS
     "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/*")
file(GLOB_RECURSE GL_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/gl/*")
file(GLOB_RECURSE JSON_FILES CONFIGURE_DEPENDS
     "${CMAKE_CURRENT_SOURCE_DIR}/json/*")
file(GLOB_RECURSE MATH_FILES CONFIGURE_DEPENDS
     "${CMAKE_CURRENT_SOURCE_DIR}/math/*")
file(GLOB_RECURSE MISC_FILES CONFIGURE_DEPENDS
     "${CMAKE_CURRENT_SOURCE_DIR}/misc/*")
file(GLOB_RECURSE SDL_FILES CONFIGURE_DEPENDS
     "${CMAKE_CURRENT_SOURCE_DIR}/sdl/*")
file(GLOB_RECURSE UTILS_FILES CONFIGURE_DEPENDS
     "${CMAKE_CURRENT_SOURCE_DIR}/utils/*")

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
target_link_libraries(gp PUBLIC SDL3::SDL3)
if(TARGET SDL3_ttf::SDL3_ttf)
  set(SDL3_TTF_TARGET SDL3_ttf::SDL3_ttf)
elseif(TARGET SDL3_ttf::SDL3_ttf-shared)
  set(SDL3_TTF_TARGET SDL3_ttf::SDL3_ttf-shared)
elseif(TARGET SDL3_ttf::SDL3_ttf-static)
  set(SDL3_TTF_TARGET SDL3_ttf::SDL3_ttf-static)
else()
  message(
    FATAL_ERROR
      "SDL3_ttf was found, but no supported imported target is available. Expected one of: SDL3_ttf::SDL3_ttf, SDL3_ttf::SDL3_ttf-shared, or SDL3_ttf::SDL3_ttf-static."
  )
endif()
target_link_libraries(gp PUBLIC ${SDL3_TTF_TARGET})

if(NOT EMSCRIPTEN)
  target_include_directories(gp PUBLIC ${FFMPEG_INCLUDE_DIRS})
  target_link_directories(gp PRIVATE ${FFMPEG_LIBRARY_DIRS})
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
