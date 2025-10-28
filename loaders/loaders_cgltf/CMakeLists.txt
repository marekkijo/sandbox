file(GLOB SRC_FILES *.cpp *.hpp)

add_executable(loaders_cgltf ${SRC_FILES})

target_compile_features(loaders_cgltf PRIVATE cxx_std_23)

target_link_libraries(loaders_cgltf gp)
target_link_libraries(loaders_cgltf Boost::program_options)
target_include_directories(loaders_cgltf PRIVATE ${CGLTF_INCLUDE_DIRS})

if(EMSCRIPTEN)
  target_compile_options(loaders_cgltf PRIVATE ${PRESET_COMPILE_OPTIONS})
  target_link_options(loaders_cgltf PRIVATE ${PRESET_LINK_OPTIONS})
  target_link_options(loaders_cgltf PRIVATE "-sALLOW_MEMORY_GROWTH=1")
endif()

add_target_resources(loaders_cgltf RESOURCES data)
add_target_resources(loaders_cgltf RESOURCES shaders COPY_IF_DIFFERENT)

set_target_properties(
  loaders_cgltf
  PROPERTIES FOLDER ${SOLUTION_FOLDER} VS_DEBUGGER_WORKING_DIRECTORY
                                       $<TARGET_FILE_DIR:loaders_cgltf>)

source_group(${SOURCE_GROUP_LABEL} FILES ${SRC_FILES})
