#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "../../../../public/simple_math/transformation/vector/vector_3d.h"
#include <stdio.h>

fvector_3d::fvector_3d()
	:x(0.f)
	,y(0.f)
	,z(0.f)
{
}

fvector_3d::fvector_3d(int in_a, int in_b, int in_c)
	:x(in_a)
	,y(in_b)
	,z(in_c)
{
}

fvector_3d::fvector_3d(float in_a, float in_b, float in_c)
	:x(in_a)
	,y(in_b)
	,z(in_c)
{
}

char* fvector_3d::to_string(char* buff)
{
	sprintf(buff, "(x=%f,y=%f,z=%f)", x, y,z);
	return buff;
}

float fvector_3d::dot(const fvector_3d& a)
{
	return a.x * x + a.y * y + z * a.z;
}
