#pragma once


#include "ZMath.h"

namespace Zero 
{
	namespace ZMath
	{
		struct FAABB
		{
			ZMath::vec3 Min, Max;

			FAABB()
				: Min(0.0f), Max(0.0f) {}

			FAABB(const ZMath::vec3& min, const ZMath::vec3& max)
				: Min(min), Max(max) {}

		};
	}
}