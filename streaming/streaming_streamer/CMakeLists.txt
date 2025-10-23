file(GLOB SRC_FILES *.cpp *.hpp)

add_executable(streaming_streamer ${SRC_FILES})

target_compile_features(streaming_streamer PRIVATE cxx_std_23)

target_link_libraries(streaming_streamer streaming_common)
target_link_libraries(streaming_streamer Boost::program_options)
target_link_libraries(streaming_streamer LibDataChannel::LibDataChannel)

add_target_resources(streaming_streamer RESOURCES shaders COPY_IF_DIFFERENT)

set_target_properties(
  streaming_streamer
  PROPERTIES FOLDER ${SOLUTION_FOLDER} VS_DEBUGGER_WORKING_DIRECTORY
                                       $<TARGET_FILE_DIR:streaming_streamer>)

source_group(${SOURCE_GROUP_LABEL} FILES ${SRC_FILES})
