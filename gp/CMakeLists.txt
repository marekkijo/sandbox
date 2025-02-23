# CMakeLists.txt for GP Library
#
# This CMake file is intended to create a General Purpose (GP) library, which
# contains support for various functionalities such as math operators, common
# structures used across different projects, helper functions for string
# operators, and headers for easy inclusion. It also provides higher-level
# support for projects using SDL (Simple DirectMedia Layer) and GLFW (Graphics
# Library Framework), if available.
#
# Please ensure that your project includes the necessary dependencies for SDL
# and GLFW.

file(GLOB_RECURSE SRC_FILES *.cpp *.hpp)

add_library(gp STATIC ${SRC_FILES})

target_compile_features(gp PRIVATE cxx_std_23)

target_include_directories(gp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_include_directories(gp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(gp PRIVATE ${FFMPEG_INCLUDE_DIRS})
target_link_directories(gp PRIVATE ${FFMPEG_LIBRARY_DIRS})
target_link_libraries(gp PRIVATE ${FFMPEG_LIBRARIES})
target_link_libraries(gp PRIVATE glad::glad)
target_link_libraries(gp PRIVATE glfw)
target_link_libraries(gp PRIVATE glm::glm)
target_link_libraries(gp PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(gp PRIVATE SDL2::SDL2)
target_link_libraries(gp PRIVATE $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRC_FILES})
