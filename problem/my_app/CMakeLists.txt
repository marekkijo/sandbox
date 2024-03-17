add_executable(my_app main.cpp)

target_link_libraries(my_app LibDataChannel::LibDataChannel)
target_link_libraries(my_app my_lib)

add_custom_command(TARGET my_app PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
  ${LIBDATACHANNEL_DLL_BIN} $<TARGET_FILE_DIR:my_app>
)
