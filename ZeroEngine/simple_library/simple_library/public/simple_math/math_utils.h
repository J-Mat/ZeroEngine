#pragma once
#include <vector>

//œÚ¡ø
#include "transformation/vector/vector_2d.h"
#include "transformation/vector/vector_3d.h"
#include "transformation/vector/vector_4d.h"

//æÿ’Û
#include "transformation/matrix/matrix_2x2.h"
#include "transformation/matrix/matrix_3x3.h"
#include "transformation/matrix/matrix_4x4.h"

using namespace std;
namespace math_utils
{
	//æÿ’Û≥À∑®
	fvector_3d mul(const fvector_2d &in_3d, const fmatrix_3x3& in_matrix_3x3);
	fvector_3d mul(const fvector_3d &in_3d, const fmatrix_3x3& in_matrix_3x3);

	//æÿ’Û–˝◊™
	void rot_radian(float in_radian, fmatrix_3x3& in_world_matrix_3x3);
	void rot_angle(float in_angle, fmatrix_3x3& in_world_matrix_3x3);

	//æÿ’ÛÀı∑≈
	void set_scale(const fvector_2d& in_scale, fmatrix_3x3& in_world_matrix_3x3);
	fmatrix_3x3 set_scale(const fvector_2d& in_scale);
}