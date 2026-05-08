set(SOLUTION_FOLDER streaming)

option(STREAMING_PIPELINE_STATS
       "Enable per-frame pipeline timing stats (zero overhead when OFF)" ON)

add_subdirectory(streaming_common)
add_subdirectory(streaming_encode_decode)
add_subdirectory(streaming_receiver)
add_subdirectory(streaming_signaling_server)
add_subdirectory(streaming_streamer)
