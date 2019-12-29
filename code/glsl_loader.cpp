
#define _CRT_SECURE_NO_WARNINGS

#include "main_header.h"
#include "glsl_loader.h"

const char *load_glsl(const char *path)
{
	FILE *hFile = fopen(path, "r");
	if (nullptr != hFile)
	{
		// FIXME: read & allocate text
	}

	return nullptr;
}
