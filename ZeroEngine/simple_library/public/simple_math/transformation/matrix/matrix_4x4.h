#pragma once

struct fmatrix_4x4
{
	float m11; float m12; float m13; float m14;
	float m21; float m22; float m23; float m24;
	float m31; float m32; float m33; float m34;
	float m41; float m42; float m43; float m44;

public:
	fmatrix_4x4();

	fmatrix_4x4(
		float in_m11, float in_m12, float in_m13, float in_m14,
		float in_m21, float in_m22, float in_m23, float in_m24,
		float in_m31, float in_m32, float in_m33, float in_m34,
		float in_m41, float in_m42, float in_m43, float in_m44);

	void operator+=(const fmatrix_4x4& a)
	{
		*this = *this + a;
	}

	fmatrix_4x4 operator+(const fmatrix_4x4& a)
	{
		return fmatrix_4x4(
			m11 + a.m11, m12 + a.m12, m13 + a.m13, m14 + a.m14,
			m21 + a.m21, m22 + a.m22, m23 + a.m23, m24 + a.m24,
			m31 + a.m31, m32 + a.m32, m33 + a.m33, m34 + a.m34,
			m41 + a.m41, m42 + a.m42, m43 + a.m43, m44 + a.m44);
	}

	void operator-=(const fmatrix_4x4& a)
	{
		*this = *this - a;
	}

	fmatrix_4x4 operator-(const fmatrix_4x4& a)
	{
		return fmatrix_4x4(
			m11 - a.m11, m12 - a.m12, m13 - a.m13, m14 - a.m14,
			m21 - a.m21, m22 - a.m22, m23 - a.m23, m24 - a.m24,
			m31 - a.m31, m32 - a.m32, m33 - a.m33, m34 - a.m34,
			m41 - a.m41, m42 - a.m42, m43 - a.m43, m44 - a.m44);
	}

	//µ¥Î»»¯
	static fmatrix_4x4 identity();
};