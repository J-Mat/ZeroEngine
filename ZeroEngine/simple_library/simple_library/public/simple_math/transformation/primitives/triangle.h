#pragma once
#include "../vector/vector_2d.h"
#include "../vector/vector_4d.h"

namespace primitives
{
	struct ftriangle
	{
		fvector_2d point_1;
		fvector_2d point_2;
		fvector_2d point_3;

		fvector_4d Color;
	};

	//�ж�һ�����ǲ���������������
	bool is_inside_triangle(ftriangle in_triangle, int x, int y);
}