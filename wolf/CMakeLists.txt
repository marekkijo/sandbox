set(SOLUTION_FOLDER wolf)
set(DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/data)

add_subdirectory(wolf_multithread)
add_subdirectory(wolf_singlethread)
add_subdirectory(wolf_common)
