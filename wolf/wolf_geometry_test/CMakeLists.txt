file(GLOB SRC_FILES *.cpp *.hpp)

add_executable(wolf_geometry_test ${SRC_FILES})

target_compile_features(wolf_geometry_test PRIVATE cxx_std_23)

target_link_libraries(wolf_geometry_test wolf_common)
target_link_libraries(wolf_geometry_test Boost::program_options)

if(EMSCRIPTEN)
  target_compile_options(wolf_geometry_test PRIVATE ${PRESET_COMPILE_OPTIONS})
  target_link_options(wolf_geometry_test PRIVATE ${PRESET_LINK_OPTIONS})
endif()

add_target_resources(wolf_geometry_test RESOURCES ${DATA_DIR} DESTINATIONS data)

set_target_properties(
  wolf_geometry_test
  PROPERTIES FOLDER ${SOLUTION_FOLDER} VS_DEBUGGER_WORKING_DIRECTORY
                                       $<TARGET_FILE_DIR:wolf_geometry_test>)

source_group(${SOURCE_GROUP_LABEL} FILES ${SRC_FILES})
