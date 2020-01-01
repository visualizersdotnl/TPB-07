
#include "main-header.h"
#include "text-loader.h"

static std::vector<const char *> s_texts;

const char *load_plain_text(const char *path)
{
	FILE *hFile = fopen(path, "r");
	if (nullptr != hFile)
	{
		// FIXME: read & allocate text
	}

	return nullptr;
}

void release_all_plain_text()
{
	for (auto text : s_texts)
		delete[] text;
}
