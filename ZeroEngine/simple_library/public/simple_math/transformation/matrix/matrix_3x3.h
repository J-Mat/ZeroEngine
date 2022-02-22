#pragma once

struct fmatrix_3x3
{
	float m11; float m12; float m13;
	float m21; float m22; float m23;
	float m31; float m32; float m33;

public:
	fmatrix_3x3();

	fmatrix_3x3(
	float in_m11, float in_m12, float in_m13,
	float in_m21, float in_m22, float in_m23,
	float in_m31, float in_m32, float in_m33);

	void operator+=(const fmatrix_3x3& a)
	{
		*this = *this + a;
	}

	fmatrix_3x3 operator+(const fmatrix_3x3& a)
	{
		return fmatrix_3x3(
			m11 + a.m11, m12 + a.m12, m13 + a.m13,
			m21 + a.m21, m22 + a.m22, m23 + a.m23,
			m31 + a.m31, m32 + a.m32, m33 + a.m33);
	}

	void operator-=(const fmatrix_3x3& a)
	{
		*this = *this - a;
	}

	fmatrix_3x3 operator-(const fmatrix_3x3& a)
	{
		return fmatrix_3x3(
			m11 - a.m11, m12 - a.m12, m13 - a.m13,
			m21 - a.m21, m22 - a.m22, m23 - a.m23,
			m31 - a.m31, m32 - a.m32, m33 - a.m33);
	}

	void operator*=(const int& a)
	{
		*this = *this * a;
	}

	fmatrix_3x3 operator*(const int& a)
	{
		return fmatrix_3x3(
			m11 * a, m12 * a, m13 * a,
			m21 * a, m22 * a, m23 * a,
			m31 * a, m32 * a, m33 * a);
	}

	void operator*=(const float& a)
	{
		*this = *this * a;
	}

	fmatrix_3x3 operator*(const float& a)
	{
		return fmatrix_3x3(
			m11 * a, m12 * a, m13 * a,
			m21 * a, m22 * a, m23 * a,
			m31 * a, m32 * a, m33 * a);
	}

	void operator*=(const fmatrix_3x3& a)
	{
		*this = *this * a;
	}

	fmatrix_3x3 operator*(const fmatrix_3x3& a)
	{
		return fmatrix_3x3(
			m11 * a.m11 + m12 * a.m21 + m13 * a.m31,
			m11 * a.m12 + m12 * a.m22 + m13 * a.m32,
			m11 * a.m13 + m12 * a.m23 + m13 * a.m33,
			
			m21* a.m11 + m22 * a.m21 + m23 * a.m31,
			m21* a.m12 + m22 * a.m22 + m23 * a.m32,
			m21* a.m13 + m22 * a.m23 + m23 * a.m33,

			m31* a.m11 + m32 * a.m21 + m33 * a.m31,
			m31* a.m12 + m32 * a.m22 + m33 * a.m32,
			m31* a.m13 + m32 * a.m23 + m33 * a.m33);
	}

	//行列式
	float Determinant();

	//单位化
	static fmatrix_3x3 identity();
};