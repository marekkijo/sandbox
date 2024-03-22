file(GLOB SRC_FILES *.cpp *.hpp)

add_executable(wolf_singlethread ${SRC_FILES})

target_link_libraries(wolf_singlethread wolf_common)
target_link_libraries(wolf_singlethread SDL2::SDL2main)
target_link_libraries(wolf_singlethread Boost::program_options)

add_custom_command(
  TARGET wolf_singlethread
  PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${DATA_DIR}
          $<TARGET_FILE_DIR:wolf_singlethread>/data)

set_target_properties(
  wolf_singlethread
  PROPERTIES FOLDER ${SOLUTION_FOLDER} VS_DEBUGGER_WORKING_DIRECTORY
                                       $<TARGET_FILE_DIR:wolf_singlethread>)

source_group(${SOURCE_GROUP_LABEL} FILES ${SRC_FILES})
