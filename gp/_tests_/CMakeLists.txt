file(GLOB_RECURSE UTILS_FILES "${CMAKE_CURRENT_SOURCE_DIR}/utils/*")

add_library(utils STATIC ${UTILS_FILES})
source_group(${SOURCE_GROUP_LABEL} FILES ${UTILS_FILES})
target_compile_features(utils PRIVATE cxx_std_23)
target_link_libraries(utils PRIVATE GTest::gtest GTest::gtest_main GTest::gmock
                                    GTest::gmock_main)
target_include_directories(utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(
  utils PROPERTIES FOLDER ${SOLUTION_FOLDER}/${TESTS_DIRECTORY}
                   VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:utils>)

file(GLOB_RECURSE SDL_TESTS_FILES "${CMAKE_CURRENT_SOURCE_DIR}/sdl_tests/*")
add_test_target(sdl_tests FILES ${SDL_TESTS_FILES} DEPS utils gp)
