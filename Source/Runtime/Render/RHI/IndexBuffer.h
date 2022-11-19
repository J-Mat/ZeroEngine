#pragma once
#include "Core.h"

namespace Zero
{
	class IIndexBuffer
	{
	public:
		IIndexBuffer(uint32_t* Data, uint32_t IndexCount)
			: m_Data(Data)
			, m_IndexCount(IndexCount)
			, m_BufferSize(sizeof(uint32_t) * IndexCount)
			
		{};
		virtual ~IIndexBuffer() {}
		virtual uint32_t GetIndexCount() { return m_IndexCount; }
	protected:
		uint32_t* m_Data = 0;
		uint32_t m_IndexCount = 0;
		uint32_t m_BufferSize = 0;
	};
}