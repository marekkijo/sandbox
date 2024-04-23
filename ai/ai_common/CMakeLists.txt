file(GLOB SRC_FILES *.cpp *.hpp)

add_library(ai_common STATIC ${SRC_FILES})

target_include_directories(ai_common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_link_libraries(ai_common gp)

set_target_properties(ai_common PROPERTIES FOLDER ${SOLUTION_FOLDER})

source_group(${SOURCE_GROUP_LABEL} FILES ${SRC_FILES})
