file(GLOB SRC_FILES *.cpp *.hpp)

add_library(streaming_common STATIC ${SRC_FILES})

target_compile_features(streaming_common PRIVATE cxx_std_23)

target_include_directories(streaming_common
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_link_libraries(streaming_common gp)
target_include_directories(streaming_common PRIVATE ${FFMPEG_INCLUDE_DIRS})
target_link_directories(streaming_common PRIVATE ${FFMPEG_LIBRARY_DIRS})
target_link_libraries(streaming_common ${FFMPEG_LIBRARIES})

set_target_properties(streaming_common PROPERTIES FOLDER ${SOLUTION_FOLDER})

source_group(${SOURCE_GROUP_LABEL} FILES ${SRC_FILES})
