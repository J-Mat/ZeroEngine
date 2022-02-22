// Copyright (C) RenZhai.2022.All Rights Reserved.
#pragma once

struct fvector_4d
{
	float x;
	float y;
	float z;
	float w;

	fvector_4d()
	:x(0.f)
	,y(0.f)
	,z(0.f)
	,w(1.f)
	{

	}

	fvector_4d(float a,float b,float c,float d)
		:x(a)
		,y(b)
		,z(c)
		,w(d)
	{}
};