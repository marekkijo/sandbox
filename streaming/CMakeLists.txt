set(SOLUTION_FOLDER streaming)
set(LIBDATACHANNEL_DLL_BIN
    "$<IF:$<CONFIG:Debug>,${libdatachannel_PACKAGE_FOLDER_DEBUG}/bin,${libdatachannel_PACKAGE_FOLDER_RELEASE}/bin>"
)

add_subdirectory(streaming_common)
add_subdirectory(streaming_encode_decode)
add_subdirectory(streaming_receiver)
add_subdirectory(streaming_signaling_server)
add_subdirectory(streaming_streamer)
