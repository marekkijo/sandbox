file(GLOB SRC_FILES *.cpp *.hpp)

add_executable(wolf_sprite_voxelizer ${SRC_FILES})

target_compile_features(wolf_sprite_voxelizer PRIVATE cxx_std_23)

target_link_libraries(wolf_sprite_voxelizer gp)
target_link_libraries(wolf_sprite_voxelizer wolf_common)
target_link_libraries(wolf_sprite_voxelizer Boost::program_options)
target_link_libraries(wolf_sprite_voxelizer SDL2::SDL2)
target_link_libraries(wolf_sprite_voxelizer SDL2::SDL2main)
target_link_libraries(wolf_sprite_voxelizer glm::glm)

if(NOT DEFINED EMSCRIPTEN)
  target_link_libraries(wolf_sprite_voxelizer glad::glad)
endif()

if(DEFINED EMSCRIPTEN)
  target_compile_options(wolf_sprite_voxelizer
                         PRIVATE ${EMSCRIPTEN_COMPILE_OPTIONS})
  target_link_options(wolf_sprite_voxelizer PRIVATE ${EMSCRIPTEN_LINK_OPTIONS})

  target_link_options(
    wolf_sprite_voxelizer PRIVATE "-sASSERTIONS=1" "-sALLOW_MEMORY_GROWTH=1"
    "SHELL:--preload-file ${DATA_DIR}@/data"
    "SHELL:--preload-file ${CMAKE_CURRENT_SOURCE_DIR}/shaders@/shaders")
else()
  add_custom_command(
    TARGET wolf_sprite_voxelizer
    PRE_BUILD
    COMMAND
      ${CMAKE_COMMAND} -E copy_directory_if_different
      ${CMAKE_CURRENT_SOURCE_DIR}/shaders
      $<TARGET_FILE_DIR:wolf_sprite_voxelizer>/shaders)

  add_custom_command(
    TARGET wolf_sprite_voxelizer
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${DATA_DIR}
            $<TARGET_FILE_DIR:wolf_sprite_voxelizer>/data)
endif()

set_target_properties(
  wolf_sprite_voxelizer
  PROPERTIES FOLDER ${SOLUTION_FOLDER} VS_DEBUGGER_WORKING_DIRECTORY
                                       $<TARGET_FILE_DIR:wolf_sprite_voxelizer>)

source_group(${SOURCE_GROUP_LABEL} FILES ${SRC_FILES})
