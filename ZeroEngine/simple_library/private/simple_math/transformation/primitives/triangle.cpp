#include "../../../../public/simple_math/transformation/primitives/triangle.h"

namespace primitives
{
	bool is_inside_triangle(ftriangle in_triangle, int x, int y)
	{
		//
		if (fvector_2d::cross_product(in_triangle.point_3 - in_triangle.point_1, in_triangle.point_2 - in_triangle.point_1) >= 0.f)
		{
			fvector_2d tmp = in_triangle.point_3;
			in_triangle.point_3 = in_triangle.point_2;
			in_triangle.point_2 = tmp;
		}

		//
		if (fvector_2d::cross_product(
			in_triangle.point_2 - in_triangle.point_1, 
			fvector_2d(x, y) - in_triangle.point_1) < 0.f)
		{
			return false; //左侧 不在内部
		}

		if (fvector_2d::cross_product(
			in_triangle.point_3 - in_triangle.point_2,
			fvector_2d(x, y) - in_triangle.point_2) < 0.f)
		{
			return false;//右侧 不在内部
		}

		if (fvector_2d::cross_product(
			in_triangle.point_1 - in_triangle.point_3,
			fvector_2d(x, y) - in_triangle.point_3) < 0.f)
		{
			return false;//右侧 不在内部
		}

		return true;
	}
}