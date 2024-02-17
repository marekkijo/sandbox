set(SOLUTION_FOLDER tools)

add_library(tools_common INTERFACE
  common/user_input.hpp
)

set_target_properties(tools_common PROPERTIES FOLDER ${SOLUTION_FOLDER})

add_library(tools_math STATIC
  math/math.cpp
  math/math.hpp
)

set_target_properties(tools_math PROPERTIES FOLDER ${SOLUTION_FOLDER})

add_library(tools_sdl STATIC
  sdl/sdl_animation.cpp
  sdl/sdl_animation.hpp
  sdl/sdl_gl_context.cpp
  sdl/sdl_gl_context.hpp
  sdl/sdl_keyboard_state.cpp
  sdl/sdl_keyboard_state.hpp
  sdl/sdl_renderer.cpp
  sdl/sdl_renderer.hpp
  sdl/sdl_system.cpp
  sdl/sdl_system.hpp
  sdl/sdl_window.cpp
  sdl/sdl_window.hpp
)

target_link_libraries(tools_sdl SDL2::SDL2)

set_target_properties(tools_sdl PROPERTIES FOLDER ${SOLUTION_FOLDER})

add_library(tools_utils STATIC
  utils/utils.cpp
  utils/utils.hpp
)

set_target_properties(tools_utils PROPERTIES FOLDER ${SOLUTION_FOLDER})
