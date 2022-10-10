#pragma once


#include "ZMath.h"

namespace Zero 
{
	namespace ZMath
	{
		class FAABB
		{
		public:
			ZMath::vec3 Min, Max;

			FAABB()
			{
				float MaxFloatValue = std::numeric_limits<float>::max();
				Min = { MaxFloatValue , MaxFloatValue, MaxFloatValue};
				float MinFloatValue = std::numeric_limits<float>::min();
				Max = { MinFloatValue, MinFloatValue, MinFloatValue };
			}

			FAABB(const ZMath::vec3& min, const ZMath::vec3& max)
				: Min(min), Max(max) {}
			
			void Merge(const ZMath::vec3& Pos)
			{
				Min.x = std::min(Min.x, Pos.x);
				Min.y = std::min(Min.y, Pos.y);
				Min.z = std::min(Min.z, Pos.z);

				Max.x = std::max(Max.x, Pos.x);
				Max.y = std::max(Max.y, Pos.y);
				Max.z = std::max(Max.z, Pos.y);
			}

			void Merge(const FAABB& Other)
			{
				Merge(Other.Min);
				Merge(Other.Max);
			}
		};
	}
}