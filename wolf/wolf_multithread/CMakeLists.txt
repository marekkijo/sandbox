file(GLOB SRC_FILES *.cpp *.hpp)

add_executable(wolf_multithread ${SRC_FILES})

target_link_libraries(wolf_multithread gp)
target_link_libraries(wolf_multithread wolf_common)
target_link_libraries(wolf_multithread SDL2::SDL2)
target_link_libraries(wolf_multithread SDL2::SDL2main)
target_link_libraries(wolf_multithread Boost::program_options)

add_custom_command(
  TARGET wolf_multithread
  PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${DATA_DIR}
          $<TARGET_FILE_DIR:wolf_multithread>/data)

set_target_properties(
  wolf_multithread
  PROPERTIES FOLDER ${SOLUTION_FOLDER} VS_DEBUGGER_WORKING_DIRECTORY
                                       $<TARGET_FILE_DIR:wolf_multithread>)

source_group(${SOURCE_GROUP_LABEL} FILES ${SRC_FILES})
