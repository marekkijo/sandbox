set(SOLUTION_FOLDER loaders)

if(NOT EMSCRIPTEN_EXCEPTIONS_DISABLED)
  add_subdirectory(loaders_assimp)
endif()
add_subdirectory(loaders_cgltf)
