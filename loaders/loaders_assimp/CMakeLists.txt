file(GLOB SRC_FILES *.cpp *.hpp)

add_executable(loaders_assimp ${SRC_FILES})

target_compile_features(loaders_assimp PRIVATE cxx_std_23)

target_link_libraries(loaders_assimp gp)
target_link_libraries(loaders_assimp Boost::program_options)
target_link_libraries(loaders_assimp assimp::assimp)

if(EMSCRIPTEN)
  target_compile_options(loaders_assimp PRIVATE ${PRESET_COMPILE_OPTIONS})
  target_link_options(loaders_assimp PRIVATE ${PRESET_LINK_OPTIONS})
  target_link_options(loaders_assimp PRIVATE "-sALLOW_MEMORY_GROWTH=1")
endif()

add_target_resources(loaders_assimp RESOURCES data)
add_target_resources(loaders_assimp RESOURCES shaders COPY_IF_DIFFERENT)

set_target_properties(
  loaders_assimp
  PROPERTIES FOLDER ${SOLUTION_FOLDER} VS_DEBUGGER_WORKING_DIRECTORY
                                       $<TARGET_FILE_DIR:loaders_assimp>)

source_group(${SOURCE_GROUP_LABEL} FILES ${SRC_FILES})
