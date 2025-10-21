file(GLOB SRC_FILES *.cpp *.hpp)

add_executable(ai_pong ${SRC_FILES})

target_compile_features(ai_pong PRIVATE cxx_std_23)

target_link_libraries(ai_pong gp)
target_link_libraries(ai_pong ai_common)
target_link_libraries(ai_pong Boost::program_options)
target_link_libraries(ai_pong SDL2::SDL2)
target_link_libraries(ai_pong SDL2::SDL2main)
target_link_libraries(ai_pong glm::glm)

if(EMSCRIPTEN)
  target_compile_options(ai_pong PRIVATE ${PRESET_COMPILE_OPTIONS})
  target_link_options(ai_pong PRIVATE ${PRESET_LINK_OPTIONS})
endif()

add_target_resources(ai_pong RESOURCES data)

set_target_properties(
  ai_pong PROPERTIES FOLDER ${SOLUTION_FOLDER} VS_DEBUGGER_WORKING_DIRECTORY
                                               $<TARGET_FILE_DIR:ai_pong>)

source_group(${SOURCE_GROUP_LABEL} FILES ${SRC_FILES})
