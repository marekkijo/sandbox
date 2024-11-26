file(GLOB SRC_FILES *.cpp *.hpp)

set(TARGET_NAME glfw_application_example)
add_executable(${TARGET_NAME} ${SRC_FILES})

target_link_libraries(${TARGET_NAME} gp)
target_link_libraries(${TARGET_NAME} Boost::program_options)

set_target_properties(
  ${TARGET_NAME}
  PROPERTIES FOLDER ${SOLUTION_FOLDER} VS_DEBUGGER_WORKING_DIRECTORY
                                       $<TARGET_FILE_DIR:${TARGET_NAME}>)

source_group(${SOURCE_GROUP_LABEL} FILES ${SRC_FILES})
