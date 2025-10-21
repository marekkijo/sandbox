file(GLOB SRC_FILES *.cpp *.hpp)

add_executable(streaming_receiver ${SRC_FILES})

target_compile_features(streaming_receiver PRIVATE cxx_std_23)

target_link_libraries(streaming_receiver gp)
target_link_libraries(streaming_receiver streaming_common)
target_link_libraries(streaming_receiver Boost::program_options)
target_link_libraries(streaming_receiver LibDataChannel::LibDataChannel)
target_link_libraries(streaming_receiver nlohmann_json::nlohmann_json)
target_link_libraries(streaming_receiver glm::glm)
target_link_libraries(streaming_receiver glad::glad)
target_link_libraries(
  streaming_receiver
  PRIVATE
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>)

add_custom_command(
  TARGET streaming_receiver
  PRE_BUILD
  COMMAND
    ${CMAKE_COMMAND} -E copy_directory_if_different
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders
    $<TARGET_FILE_DIR:streaming_receiver>/shaders)

set_target_properties(
  streaming_receiver
  PROPERTIES FOLDER ${SOLUTION_FOLDER} VS_DEBUGGER_WORKING_DIRECTORY
                                       $<TARGET_FILE_DIR:streaming_receiver>)

source_group(${SOURCE_GROUP_LABEL} FILES ${SRC_FILES})
