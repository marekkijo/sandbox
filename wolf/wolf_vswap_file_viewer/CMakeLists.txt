file(GLOB SRC_FILES *.cpp *.hpp)

add_executable(wolf_vswap_file_viewer ${SRC_FILES})

target_link_libraries(wolf_vswap_file_viewer gp)
target_link_libraries(wolf_vswap_file_viewer wolf_common)
target_link_libraries(wolf_vswap_file_viewer Boost::program_options)
target_link_libraries(wolf_vswap_file_viewer glad::glad)
target_link_libraries(wolf_vswap_file_viewer glfw)
target_link_libraries(wolf_vswap_file_viewer glm::glm)

add_custom_command(
  TARGET wolf_vswap_file_viewer
  PRE_BUILD
  COMMAND
    ${CMAKE_COMMAND} -E copy_directory_if_different
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders
    $<TARGET_FILE_DIR:wolf_vswap_file_viewer>/shaders)

add_custom_command(
  TARGET wolf_vswap_file_viewer
  PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${DATA_DIR}
          $<TARGET_FILE_DIR:wolf_vswap_file_viewer>/data)

set_target_properties(
  wolf_vswap_file_viewer
  PROPERTIES FOLDER ${SOLUTION_FOLDER} VS_DEBUGGER_WORKING_DIRECTORY
                                       $<TARGET_FILE_DIR:wolf_vswap_file_viewer>)

source_group(${SOURCE_GROUP_LABEL} FILES ${SRC_FILES})
