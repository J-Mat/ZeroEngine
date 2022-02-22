#include "..\..\public\simple_math\math_utils.h"

fvector_3d math_utils::mul(const fvector_2d& in_3d, const fmatrix_3x3& in_matrix_3x3)
{
    return mul(fvector_3d(in_3d.x, in_3d.y,1.f),in_matrix_3x3);
}

fvector_3d math_utils::mul(const fvector_3d& in_3d, const fmatrix_3x3& in_matrix_3x3)
{
    return fvector_3d(
     in_3d.x * in_matrix_3x3.m11 + in_3d.y * in_matrix_3x3.m21 + in_3d.z * in_matrix_3x3.m31,
     in_3d.x * in_matrix_3x3.m12 + in_3d.y * in_matrix_3x3.m22 + in_3d.z * in_matrix_3x3.m32,
     in_3d.x * in_matrix_3x3.m13 + in_3d.y * in_matrix_3x3.m23 + in_3d.z * in_matrix_3x3.m33);
}

void math_utils::rot_radian(float in_radian, fmatrix_3x3& in_world_matrix_3x3)
{
    //旋转矩阵
    fmatrix_3x3 rot_matrix_3x3;

    //公式
    rot_matrix_3x3.m11 = cos(in_radian);
    rot_matrix_3x3.m12 = -sin(in_radian);
    rot_matrix_3x3.m21 = sin(in_radian);
    rot_matrix_3x3.m22 = cos(in_radian);

        //矩阵乘法
    in_world_matrix_3x3 = rot_matrix_3x3* in_world_matrix_3x3;
}

void math_utils::rot_angle(float in_angle, fmatrix_3x3& in_world_matrix_3x3)
{
    float in_radian = in_angle * (3.1415926 / 180.0);

    rot_radian(in_radian, in_world_matrix_3x3);
}

void math_utils::set_scale(const fvector_2d& in_scale, fmatrix_3x3& in_world_matrix_3x3)
{
    in_world_matrix_3x3.m11 = in_scale.x;
    in_world_matrix_3x3.m22 = in_scale.y;
}

fmatrix_3x3 math_utils::set_scale(const fvector_2d& in_scale)
{
    fmatrix_3x3 in_world_matrix_3x3;
    set_scale(in_scale, in_world_matrix_3x3);

    return in_world_matrix_3x3;
}
