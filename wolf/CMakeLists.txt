set(SOLUTION_FOLDER wolf)
set(DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/data)

add_subdirectory(wolf_common)
add_subdirectory(wolf_multithread)
add_subdirectory(wolf_singlethread)
add_subdirectory(wolf_sprite_voxelizer)

# TODO(build): make it buildable
# TODO(build): add_subdirectory(wolf_vswap_file_viewer)
