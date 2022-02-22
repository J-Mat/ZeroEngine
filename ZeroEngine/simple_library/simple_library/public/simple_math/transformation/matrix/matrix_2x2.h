#pragma once

struct fmatrix_2x2
{
	float m11;float m12;
	float m21;float m22;

public:
	fmatrix_2x2();

	fmatrix_2x2(
		float in_m11, 
		float in_m12, 
		float in_m21, 
		float in_m22);

	//行列式
	float Determinant();
public:
	void operator+=(const fmatrix_2x2& a)
	{
		*this = *this + a;
	}

	fmatrix_2x2 operator+(const fmatrix_2x2& a)
	{
		return fmatrix_2x2(
			m11 + a.m11,
			m12 + a.m12,
			m21 + a.m21,
			m22 + a.m22);
	}

	void operator-=(const fmatrix_2x2& a)
	{
		*this = *this - a;
	}

	fmatrix_2x2 operator-(const fmatrix_2x2& a)
	{
		return fmatrix_2x2(
			m11 - a.m11,
			m12 - a.m12,
			m21 - a.m21,
			m22 - a.m22);
	}

	void operator*=(const int& a)
	{
		*this = *this * a;
	}

	fmatrix_2x2 operator*(const int& a)
	{
		return fmatrix_2x2(
			m11 * a,
			m12 * a,
			m21 * a,
			m22 * a);
	}

	void operator*=(const float& a)
	{
		*this = *this * a;
	}

	fmatrix_2x2 operator*(const float& a)
	{
		return fmatrix_2x2(
			m11 * a,
			m12 * a,
			m21 * a,
			m22 * a);
	}

	void operator*=(const fmatrix_2x2& a)
	{
		*this = *this * a;
	}

	fmatrix_2x2 operator*(const fmatrix_2x2& a)
	{
		return fmatrix_2x2(
			m11 * a.m11 + m12 * a.m21, m11 * a.m12 + m12 * a.m22,	
			m21 * a.m11 + m22 * a.m21, m21 * a.m12 + m22 * a.m22);
	}
public:
	//单位化
	static fmatrix_2x2 identity();
};