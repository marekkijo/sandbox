file(GLOB SRC_FILES *.cpp *.hpp)

add_executable(streaming_signaling_server ${SRC_FILES})

target_link_libraries(streaming_signaling_server gp)
target_link_libraries(streaming_signaling_server streaming_common)
target_link_libraries(streaming_signaling_server Boost::program_options)
target_link_libraries(streaming_signaling_server LibDataChannel::LibDataChannel)
target_link_libraries(streaming_signaling_server nlohmann_json::nlohmann_json)

add_custom_command(TARGET streaming_signaling_server PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
  ${LIBDATACHANNEL_DLL_BIN} $<TARGET_FILE_DIR:streaming_signaling_server>
)

set_target_properties(streaming_signaling_server PROPERTIES
  FOLDER ${SOLUTION_FOLDER}
  VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:streaming_signaling_server>
)

source_group(${SOURCE_GROUP_LABEL} FILES ${SRC_FILES})
