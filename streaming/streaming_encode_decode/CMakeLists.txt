file(GLOB SRC_FILES *.cpp *.hpp)

add_executable(streaming_encode_decode ${SRC_FILES})

target_link_libraries(streaming_encode_decode gp)
target_link_libraries(streaming_encode_decode streaming_common)
target_link_libraries(streaming_encode_decode SDL2::SDL2)
target_link_libraries(streaming_encode_decode SDL2::SDL2main)
target_link_libraries(streaming_encode_decode Boost::program_options)
target_link_libraries(streaming_encode_decode fmt::fmt)
target_link_libraries(streaming_encode_decode glm::glm)
target_link_libraries(streaming_encode_decode glad::glad)
target_link_libraries(streaming_encode_decode glfw)

set_target_properties(streaming_encode_decode PROPERTIES
  FOLDER ${SOLUTION_FOLDER}  
  VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:streaming_encode_decode>
)

source_group(${SOURCE_GROUP_LABEL} FILES ${SRC_FILES})
