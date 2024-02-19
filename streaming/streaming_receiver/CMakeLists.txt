file(GLOB SRC_FILES *.cpp *.hpp)

add_executable(streaming_receiver ${SRC_FILES})

target_link_libraries(streaming_receiver gp)
target_link_libraries(streaming_receiver streaming_common)
target_link_libraries(streaming_receiver SDL2::SDL2)
target_link_libraries(streaming_receiver SDL2::SDL2main)
target_link_libraries(streaming_receiver Boost::program_options)
target_link_libraries(streaming_receiver openssl::openssl)
target_link_libraries(streaming_receiver libdatachannel::libdatachannel)
target_link_libraries(streaming_receiver nlohmann_json::nlohmann_json)

add_custom_command(TARGET streaming_receiver PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
  ${LIBDATACHANNEL_DLL_BIN} $<TARGET_FILE_DIR:streaming_receiver>
)

set_target_properties(streaming_receiver PROPERTIES
  FOLDER ${SOLUTION_FOLDER}
  VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:streaming_receiver>
)

source_group(${SOURCE_GROUP_LABEL} FILES ${SRC_FILES})
