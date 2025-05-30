file(GLOB SRC_FILES *.cpp *.hpp)

add_executable(loaders_cgltf ${SRC_FILES})

target_compile_features(loaders_cgltf PRIVATE cxx_std_23)

target_link_libraries(loaders_cgltf PRIVATE gp)
target_link_libraries(loaders_cgltf PRIVATE Boost::program_options)
target_link_libraries(loaders_cgltf PRIVATE glad::glad)
target_link_libraries(loaders_cgltf PRIVATE glfw)
target_link_libraries(loaders_cgltf PRIVATE glm::glm)
target_include_directories(loaders_cgltf PRIVATE ${CGLTF_INCLUDE_DIRS})

add_custom_command(
  TARGET loaders_cgltf
  PRE_BUILD
  COMMAND
    ${CMAKE_COMMAND} -E copy_directory_if_different
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders
    $<TARGET_FILE_DIR:loaders_cgltf>/shaders)

add_custom_command(
  TARGET loaders_cgltf
  PRE_BUILD
  COMMAND
    ${CMAKE_COMMAND} -E copy_directory_if_different
    ${CMAKE_CURRENT_SOURCE_DIR}/data $<TARGET_FILE_DIR:loaders_cgltf>/data)

set_target_properties(
  loaders_cgltf
  PROPERTIES FOLDER ${SOLUTION_FOLDER} VS_DEBUGGER_WORKING_DIRECTORY
                                       $<TARGET_FILE_DIR:loaders_cgltf>)

source_group(${SOURCE_GROUP_LABEL} FILES ${SRC_FILES})
