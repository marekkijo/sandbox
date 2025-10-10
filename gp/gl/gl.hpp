#pragma once

#ifdef __EMSCRIPTEN__
# include <GLES3/gl3.h>
#else
# include <SDL.h>
# include <glad/glad.h>
#endif
