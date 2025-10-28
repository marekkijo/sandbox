file(GLOB SRC_FILES *.cpp *.hpp)

add_executable(streaming_signaling_server ${SRC_FILES})

target_compile_features(streaming_signaling_server PRIVATE cxx_std_23)

target_link_libraries(streaming_signaling_server streaming_common)
target_link_libraries(streaming_signaling_server Boost::program_options)
target_link_libraries(streaming_signaling_server LibDataChannel::LibDataChannel)

set_target_properties(
  streaming_signaling_server
  PROPERTIES FOLDER ${SOLUTION_FOLDER}
             VS_DEBUGGER_WORKING_DIRECTORY
             $<TARGET_FILE_DIR:streaming_signaling_server>)

source_group(${SOURCE_GROUP_LABEL} FILES ${SRC_FILES})
