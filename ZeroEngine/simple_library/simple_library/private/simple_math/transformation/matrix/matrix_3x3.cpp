#include "../../../../public/simple_math/transformation/matrix/matrix_3x3.h"

fmatrix_3x3::fmatrix_3x3()
{
	*this = fmatrix_3x3::identity();
}

fmatrix_3x3::fmatrix_3x3(
	float in_m11, float in_m12, float in_m13, 
	float in_m21, float in_m22, float in_m23,
	float in_m31, float in_m32, float in_m33)
	:m11(in_m11), m12(in_m12), m13(in_m13)
	,m21(in_m21), m22(in_m22), m23(in_m23)
	,m31(in_m31), m32(in_m32), m33(in_m33)
{
}

float fmatrix_3x3::Determinant()
{
	return	m11 * m22 * m33 +
			m12 * m23 * m31 + 
			m13 * m21 * m32 -
			m11 * m23 * m32 - 
			m12 * m21 * m33 - 
			m13 * m22 * m31;
}

fmatrix_3x3 fmatrix_3x3::identity()
{
	return fmatrix_3x3(
	1.f,0.f,0.f,
	0.f,1.f,0.f,
	0.f,0.f,1.f);
}
