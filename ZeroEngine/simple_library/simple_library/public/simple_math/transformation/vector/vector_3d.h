// Copyright (C) RenZhai.2022.All Rights Reserved.
#pragma once
#include "assert.h" 

struct fvector_3d
{
	float x;
	float y;
	float z;

public:
	fvector_3d();
	fvector_3d(int in_a, int in_b, int in_c);
	fvector_3d(float in_a, float in_b, float in_c);

	char* to_string(char* buff);
public:
	fvector_3d operator+(const fvector_3d& a)
	{
		return fvector_3d(a.x + x, a.y + y,a.z + y);
	}

	fvector_3d operator-(const fvector_3d& a)
	{
		return fvector_3d(x - a.x,y- a.y,y- a.z);
	}

	void operator-=(const fvector_3d& a)
	{
		*this = *this - a;
	}

	void operator+=(const fvector_3d& a)
	{
		*this = *this + a;
	}

	//向量不能这么做
	void operator+=(const float& a)
	{
		x += a;
		y += a;
		z += a;
	}

	//向量不能这么做
	void operator-=(const float& a)
	{
		x -= a;
		y -= a;
		z -= a;
	}

	fvector_3d operator+(const float& a)
	{
		return fvector_3d(a + x, a + y, a + y);
	}

	fvector_3d operator-(const float& a)
	{
		return fvector_3d(x - a, y - a, y - a);
	}

	fvector_3d operator*(const float& a)
	{
		return fvector_3d(a * x, a * y, a * y);
	}

	fvector_3d operator/(const float& a)
	{
		assert(a != 0.f);

		return fvector_3d(x / a, y / a, y / a);
	}

	fvector_3d operator*(const fvector_3d& a)
	{
		return fvector_3d(a.x * x, a.x * y, a.x * y);
	}

	fvector_3d operator/(const fvector_3d& a)
	{
		assert(x != 0.f);
		assert(y != 0.f);
		assert(z != 0.f);

		return fvector_3d(a.x / x, a.x / y, a.x / y);
	}

	fvector_3d operator*=(const fvector_3d& a)
	{
		*this = *this * a;
	}

	fvector_3d operator/=(const fvector_3d& a)
	{
		*this = *this / a;
	}

	fvector_3d operator^(const fvector_3d& a)
	{
		return fvector_3d(a.y * z - a.z * y,x*a.z - a.x*z,a.x*y - a.y * x);
	}

	float dot(const fvector_3d& a);
};