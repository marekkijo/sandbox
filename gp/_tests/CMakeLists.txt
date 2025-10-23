file(GLOB_RECURSE SDL_TESTS_FILES "${CMAKE_CURRENT_SOURCE_DIR}/sdl_tests/*")

add_executable(gp_sdl_tests ${SRC_FILES})

target_compile_features(gp_sdl_tests PRIVATE cxx_std_23)

target_link_libraries(gp_sdl_tests PRIVATE gp)
target_link_libraries(gp_sdl_tests PRIVATE GTest::gtest GTest::gtest_main
                                           GTest::gmock GTest::gmock_main)

set_target_properties(
  gp_sdl_tests
  PROPERTIES FOLDER ${SOLUTION_FOLDER} VS_DEBUGGER_WORKING_DIRECTORY
                                       $<TARGET_FILE_DIR:gp_sdl_tests>)

source_group(${SOURCE_GROUP_LABEL} FILES ${SRC_FILES})

add_test(NAME gp_sdl_tests COMMAND gp_sdl_tests)
