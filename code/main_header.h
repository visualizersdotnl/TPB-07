
// ** include first in every source file **

#pragma once

#define _CRT_SECURE_NO_WARNINGS // shut up about those new safer functions
#define GL_GLEXT_PROTOTYPES

// OpenGL + SDL
#include "../3rdparty/glew-2.1.0-win32/glew-2.1.0/include/GL/glew.h"
#include "../3rdparty/SDL2-devel-2.0.10-VC/SDL2-2.0.10/include/SDL.h"
#undef main // https://stackoverflow.com/questions/6847360/error-lnk2019-unresolved-external-symbol-main-referenced-in-function-tmainc
#include "../3rdparty/SDL2-devel-2.0.10-VC/SDL2-2.0.10/include/SDL_opengl.h"

// CRT + STL
#include <stdio.h>
#include <string.h>
