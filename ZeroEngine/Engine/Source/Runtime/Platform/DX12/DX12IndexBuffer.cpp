#include "DX12IndexBuffer.h"

namespace Zero
{
	FDX12IndexBuffer::FDX12IndexBuffer(unsigned int* Indices, uint32_t Count)
		: FIndexBuffer(Indices, Count)
	{
	}

	void FDX12IndexBuffer::SetData(unsigned int* Indices, UINT32 Count)
	{
	}
}