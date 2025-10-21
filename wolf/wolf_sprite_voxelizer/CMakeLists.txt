file(GLOB SRC_FILES *.cpp *.hpp)

add_executable(wolf_sprite_voxelizer ${SRC_FILES})

target_compile_features(wolf_sprite_voxelizer PRIVATE cxx_std_23)

target_link_libraries(wolf_sprite_voxelizer gp)
target_link_libraries(wolf_sprite_voxelizer wolf_common)
target_link_libraries(wolf_sprite_voxelizer Boost::program_options)
target_link_libraries(wolf_sprite_voxelizer SDL2::SDL2)
target_link_libraries(wolf_sprite_voxelizer SDL2::SDL2main)
target_link_libraries(wolf_sprite_voxelizer glm::glm)

if(NOT EMSCRIPTEN)
  target_link_libraries(wolf_sprite_voxelizer glad::glad)
endif()

if(EMSCRIPTEN)
  target_compile_options(wolf_sprite_voxelizer
                         PRIVATE ${PRESET_COMPILE_OPTIONS})
  target_link_options(wolf_sprite_voxelizer PRIVATE ${PRESET_LINK_OPTIONS})
  target_link_options(wolf_sprite_voxelizer PRIVATE "-sALLOW_MEMORY_GROWTH=1")
endif()

add_target_resources(wolf_sprite_voxelizer RESOURCES ${DATA_DIR} DESTINATIONS
                     data)
add_target_resources(wolf_sprite_voxelizer RESOURCES shaders COPY_IF_DIFFERENT)

set_target_properties(
  wolf_sprite_voxelizer
  PROPERTIES FOLDER ${SOLUTION_FOLDER} VS_DEBUGGER_WORKING_DIRECTORY
                                       $<TARGET_FILE_DIR:wolf_sprite_voxelizer>)

source_group(${SOURCE_GROUP_LABEL} FILES ${SRC_FILES})
