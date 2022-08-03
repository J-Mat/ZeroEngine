#pragma once
#include "Core.h"

namespace Zero
{
	class FIndexBuffer
	{
	public:
		FIndexBuffer(unsigned int* indices, uint32_t iCount) {};
		virtual ~FIndexBuffer() {}
		virtual uint32_t Count() = 0;
	};
}