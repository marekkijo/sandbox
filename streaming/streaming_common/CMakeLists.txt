file(GLOB SRC_FILES *.cpp *.hpp)

add_library(streaming_common STATIC ${SRC_FILES})

target_include_directories(streaming_common
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_link_libraries(streaming_common gp)
target_link_libraries(streaming_common ffmpeg::avcodec)
target_link_libraries(streaming_common ffmpeg::avformat)
target_link_libraries(streaming_common ffmpeg::avutil)
target_link_libraries(streaming_common ffmpeg::swscale)

set_target_properties(streaming_common PROPERTIES FOLDER ${SOLUTION_FOLDER})

source_group(${SOURCE_GROUP_LABEL} FILES ${SRC_FILES})
