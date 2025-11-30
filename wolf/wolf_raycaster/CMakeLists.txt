file(GLOB SRC_FILES *.cpp *.hpp)

add_executable(wolf_raycaster ${SRC_FILES})

target_compile_features(wolf_raycaster PRIVATE cxx_std_23)

target_link_libraries(wolf_raycaster wolf_common)
target_link_libraries(wolf_raycaster Boost::program_options)

if(EMSCRIPTEN)
  target_compile_options(wolf_raycaster PRIVATE ${PRESET_COMPILE_OPTIONS})
  target_link_options(wolf_raycaster PRIVATE ${PRESET_LINK_OPTIONS})
endif()

add_target_resources(wolf_raycaster RESOURCES ${DATA_DIR} DESTINATIONS data)

set_target_properties(
  wolf_raycaster
  PROPERTIES FOLDER ${SOLUTION_FOLDER} VS_DEBUGGER_WORKING_DIRECTORY
                                       $<TARGET_FILE_DIR:wolf_raycaster>)

source_group(${SOURCE_GROUP_LABEL} FILES ${SRC_FILES})
