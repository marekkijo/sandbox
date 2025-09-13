file(GLOB SRC_FILES *.cpp *.hpp)

add_library(wolf_common STATIC ${SRC_FILES})

target_compile_features(wolf_common PRIVATE cxx_std_23)

target_include_directories(wolf_common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_link_libraries(wolf_common gp)
target_link_libraries(wolf_common glm::glm)
target_link_libraries(wolf_common SDL2::SDL2)

if(DEFINED EMSCRIPTEN)
  target_compile_options(wolf_common PRIVATE ${EMSCRIPTEN_COMPILE_OPTIONS})
  target_link_options(wolf_common PRIVATE ${EMSCRIPTEN_LINK_OPTIONS})
endif()

set_target_properties(wolf_common PROPERTIES FOLDER ${SOLUTION_FOLDER})

source_group(${SOURCE_GROUP_LABEL} FILES ${SRC_FILES})
