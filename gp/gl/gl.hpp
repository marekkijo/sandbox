#pragma once

#ifdef __EMSCRIPTEN__
# include <GLES3/gl3.h>
#else
# include <gp/sdl/sdl.hpp>

# include <glad/glad.h>
#endif
