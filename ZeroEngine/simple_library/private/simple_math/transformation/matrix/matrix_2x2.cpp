#include "../../../../public/simple_math/transformation/matrix/matrix_2x2.h"

fmatrix_2x2::fmatrix_2x2()
{
	*this = fmatrix_2x2::identity();
}

fmatrix_2x2::fmatrix_2x2(float in_m11, float in_m12, float in_m21, float in_m22)
	:m11(in_m11)
	,m12(in_m12)
	,m21(in_m21)
	,m22(in_m22)
{
}

float fmatrix_2x2::Determinant()
{
	return m11*m22 - m12*m21;
}

fmatrix_2x2 fmatrix_2x2::identity()
{
	return fmatrix_2x2(1,0,
					   0,1);
}
