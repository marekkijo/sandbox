file(GLOB SRC_FILES *.cpp *.hpp)

add_executable(wolf_vswap_file_viewer ${SRC_FILES})

target_compile_features(wolf_vswap_file_viewer PRIVATE cxx_std_23)

target_link_libraries(wolf_vswap_file_viewer gp)
target_link_libraries(wolf_vswap_file_viewer wolf_common)
target_link_libraries(wolf_vswap_file_viewer Boost::program_options)
target_link_libraries(wolf_vswap_file_viewer SDL2::SDL2)
target_link_libraries(wolf_vswap_file_viewer SDL2::SDL2main)
target_link_libraries(wolf_vswap_file_viewer glm::glm)

if(NOT EMSCRIPTEN)
  target_link_libraries(wolf_vswap_file_viewer glad::glad)
endif()

if(EMSCRIPTEN)
  target_compile_options(wolf_vswap_file_viewer
                         PRIVATE ${PRESET_COMPILE_OPTIONS})
  target_link_options(wolf_vswap_file_viewer PRIVATE ${PRESET_LINK_OPTIONS})
  target_link_options(wolf_vswap_file_viewer PRIVATE "-sALLOW_MEMORY_GROWTH=1")
endif()

add_target_resources(wolf_vswap_file_viewer RESOURCES ${DATA_DIR} DESTINATIONS
                     data)
add_target_resources(wolf_vswap_file_viewer RESOURCES shaders COPY_IF_DIFFERENT)

set_target_properties(
  wolf_vswap_file_viewer
  PROPERTIES FOLDER ${SOLUTION_FOLDER}
             VS_DEBUGGER_WORKING_DIRECTORY
             $<TARGET_FILE_DIR:wolf_vswap_file_viewer>)

source_group(${SOURCE_GROUP_LABEL} FILES ${SRC_FILES})
