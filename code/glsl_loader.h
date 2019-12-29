
#pragma once

// delete pointer yourself, if nullptr returned it has failed (printf() will be called)
const char *load_glsl(const char *path);
