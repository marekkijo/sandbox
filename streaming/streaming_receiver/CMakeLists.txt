file(GLOB SRC_FILES *.cpp *.hpp)

add_executable(streaming_receiver ${SRC_FILES})

target_compile_features(streaming_receiver PRIVATE cxx_std_23)

target_link_libraries(streaming_receiver streaming_common)
target_link_libraries(streaming_receiver Boost::program_options)
target_link_libraries(streaming_receiver LibDataChannel::LibDataChannel)

add_target_resources(streaming_receiver RESOURCES shaders COPY_IF_DIFFERENT)

set_target_properties(
  streaming_receiver
  PROPERTIES FOLDER ${SOLUTION_FOLDER} VS_DEBUGGER_WORKING_DIRECTORY
                                       $<TARGET_FILE_DIR:streaming_receiver>)

source_group(${SOURCE_GROUP_LABEL} FILES ${SRC_FILES})
