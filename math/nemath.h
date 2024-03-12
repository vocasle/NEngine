#pragma once

#include "defines.h"

struct NE_Vec2
{
	float x;
	float y;
};

struct NE_Vec3
{
	float x;
	float y;
	float z;
};

struct NE_Vec4
{
	float x;
	float y;
	float z;
	float w;
};

NE_API void ne_math_print_version(void);
