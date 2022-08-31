#pragma once
#include "Core.h"

namespace Zero
{
	class IIndexBuffer
	{
	public:
		IIndexBuffer(unsigned int* indices, uint32_t iCount) {};
		virtual ~IIndexBuffer() {}
		virtual uint32_t Count() = 0;
	};
}