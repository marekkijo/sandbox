file(GLOB SRC_FILES *.cpp *.hpp)

add_executable(streaming_encode_decode ${SRC_FILES})

target_compile_features(streaming_encode_decode PRIVATE cxx_std_23)

target_link_libraries(streaming_encode_decode streaming_common)
target_link_libraries(streaming_encode_decode Boost::program_options)

add_target_resources(streaming_encode_decode RESOURCES shaders
                     COPY_IF_DIFFERENT)

set_target_properties(
  streaming_encode_decode
  PROPERTIES FOLDER ${SOLUTION_FOLDER}
             VS_DEBUGGER_WORKING_DIRECTORY
             $<TARGET_FILE_DIR:streaming_encode_decode>)

source_group(${SOURCE_GROUP_LABEL} FILES ${SRC_FILES})
