include_directories(${PROJECT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(assimp_loader
  assimp_loader/main.cpp
  assimp_loader/model_format.hpp
  assimp_loader/model.cpp
  assimp_loader/model.hpp
  assimp_loader/renderer.cpp
  assimp_loader/renderer.hpp
)

target_link_libraries(assimp_loader tools_common)
target_link_libraries(assimp_loader tools_sdl)
target_link_libraries(assimp_loader tools_utils)
target_link_libraries(assimp_loader SDL2::SDL2)
target_link_libraries(assimp_loader SDL2::SDL2main)
target_link_libraries(assimp_loader Boost::program_options)
target_link_libraries(assimp_loader opengl::opengl)
target_link_libraries(assimp_loader glm::glm)
target_link_libraries(assimp_loader GLEW::GLEW)
target_link_libraries(assimp_loader assimp::assimp)
target_link_libraries(assimp_loader imgui::imgui)

configure_file(assimp_loader/shader_program.fs ${CMAKE_CURRENT_BINARY_DIR}/shader_program.fs COPYONLY)
configure_file(assimp_loader/shader_program.vs ${CMAKE_CURRENT_BINARY_DIR}/shader_program.vs COPYONLY)
