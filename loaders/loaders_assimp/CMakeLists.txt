file(GLOB SRC_FILES *.cpp *.hpp)

add_executable(loaders_assimp ${SRC_FILES})

target_link_libraries(loaders_assimp gp)
target_link_libraries(loaders_assimp Boost::program_options)
target_link_libraries(loaders_assimp glad::glad)
target_link_libraries(loaders_assimp glfw)
target_link_libraries(loaders_assimp glm::glm)
target_link_libraries(loaders_assimp assimp::assimp)

add_custom_command(TARGET loaders_assimp PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
  ${CMAKE_CURRENT_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:loaders_assimp>/shaders
)

add_custom_command(TARGET loaders_assimp PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
  ${CMAKE_CURRENT_SOURCE_DIR}/data $<TARGET_FILE_DIR:loaders_assimp>/data
)

set_target_properties(loaders_assimp PROPERTIES
  FOLDER ${SOLUTION_FOLDER}
  VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:loaders_assimp>
)

source_group(${SOURCE_GROUP_LABEL} FILES ${SRC_FILES})
