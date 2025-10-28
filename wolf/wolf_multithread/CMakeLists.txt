file(GLOB SRC_FILES *.cpp *.hpp)

add_executable(wolf_multithread ${SRC_FILES})

target_compile_features(wolf_multithread PRIVATE cxx_std_23)

target_link_libraries(wolf_multithread wolf_common)
target_link_libraries(wolf_multithread Boost::program_options)

if(EMSCRIPTEN)
  target_compile_options(wolf_multithread PRIVATE ${PRESET_COMPILE_OPTIONS})
  target_link_options(wolf_multithread PRIVATE ${PRESET_LINK_OPTIONS})
  target_link_options(wolf_multithread PRIVATE "-sPTHREAD_POOL_SIZE=8")
endif()

add_target_resources(wolf_multithread RESOURCES ${DATA_DIR} DESTINATIONS data)

set_target_properties(
  wolf_multithread
  PROPERTIES FOLDER ${SOLUTION_FOLDER} VS_DEBUGGER_WORKING_DIRECTORY
                                       $<TARGET_FILE_DIR:wolf_multithread>)

source_group(${SOURCE_GROUP_LABEL} FILES ${SRC_FILES})
