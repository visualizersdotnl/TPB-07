
// ** Include first in every source file! **

#pragma once

// STFU about those new safer functions
#define _CRT_SECURE_NO_WARNINGS 

// ?
#define GL_GLEXT_PROTOTYPES

// OpenGL + SDL
#include "../3rdparty/glew-2.1.0-win32/glew-2.1.0/include/GL/glew.h"
#include "../3rdparty/SDL2-devel-2.0.10-VC/SDL2-2.0.10/include/SDL.h"
#undef main // https://stackoverflow.com/questions/6847360/error-lnk2019-unresolved-external-symbol-main-referenced-in-function-tmainc
#include "../3rdparty/SDL2-devel-2.0.10-VC/SDL2-2.0.10/include/SDL_opengl.h"

// CRT & STL
#include <stdint.h>
#include <math.h>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <thread>

// OpenMP
#include <omp.h>

// To see what leaks (keep it civil)
#define WIN32_CRT_LEAK_CHECK
#define WIN32_CRT_BREAK_ALLOC -1

// Resolution
const size_t kResX = 1980;
const size_t kResY = 1080;
