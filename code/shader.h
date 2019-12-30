
#pragma once

// we need 3 types:
// - regular (to (frame)buffer)
// - to render target #1 (might just be the same as regular)
// - to cube map #1
// - a 2D shader for sprites (with fixed path perhaps)

enum ShaderType
{
	ST_TO_CUBE,
	ST_TO_BUFFER
};

