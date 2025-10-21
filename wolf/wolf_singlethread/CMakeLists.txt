file(GLOB SRC_FILES *.cpp *.hpp)

add_executable(wolf_singlethread ${SRC_FILES})

target_compile_features(wolf_singlethread PRIVATE cxx_std_23)

target_link_libraries(wolf_singlethread gp)
target_link_libraries(wolf_singlethread wolf_common)
target_link_libraries(wolf_singlethread Boost::program_options)
target_link_libraries(wolf_singlethread SDL2::SDL2)
target_link_libraries(wolf_singlethread SDL2::SDL2main)

if(EMSCRIPTEN)
  target_compile_options(wolf_singlethread PRIVATE ${PRESET_COMPILE_OPTIONS})
  target_link_options(wolf_singlethread PRIVATE ${PRESET_LINK_OPTIONS})
endif()

add_target_resources(wolf_singlethread RESOURCES ${DATA_DIR} DESTINATIONS data)

set_target_properties(
  wolf_singlethread
  PROPERTIES FOLDER ${SOLUTION_FOLDER} VS_DEBUGGER_WORKING_DIRECTORY
                                       $<TARGET_FILE_DIR:wolf_singlethread>)

source_group(${SOURCE_GROUP_LABEL} FILES ${SRC_FILES})
