file(GLOB SRC_FILES *.cpp *.hpp)

add_executable(streaming_streamer ${SRC_FILES})

target_compile_features(streaming_streamer PRIVATE cxx_std_23)

target_link_libraries(streaming_streamer gp)
target_link_libraries(streaming_streamer streaming_common)
target_link_libraries(streaming_streamer Boost::program_options)
target_link_libraries(streaming_streamer LibDataChannel::LibDataChannel)
target_link_libraries(streaming_streamer nlohmann_json::nlohmann_json)
target_link_libraries(streaming_streamer glm::glm)
target_link_libraries(streaming_streamer glad::glad)
target_link_libraries(streaming_streamer glfw)

add_custom_command(
  TARGET streaming_streamer
  PRE_BUILD
  COMMAND
    ${CMAKE_COMMAND} -E copy_directory_if_different
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders
    $<TARGET_FILE_DIR:streaming_streamer>/shaders)

set_target_properties(
  streaming_streamer
  PROPERTIES FOLDER ${SOLUTION_FOLDER} VS_DEBUGGER_WORKING_DIRECTORY
                                       $<TARGET_FILE_DIR:streaming_streamer>)

source_group(${SOURCE_GROUP_LABEL} FILES ${SRC_FILES})
