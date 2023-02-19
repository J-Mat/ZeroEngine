#pragma once
#include "Core.h"
#include "Buffer.h"

namespace Zero
{
	class FIndexBuffer : public FBuffer
	{
	public:
		FIndexBuffer(void* Data, const FBufferDesc& Desc)
			: FBuffer(Data, Desc)
		{};
		virtual ~FIndexBuffer() {}
		virtual uint32_t GetIndexCount() { return m_Desc.Count; }
	};
}