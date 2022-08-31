#include "DX12IndexBuffer.h"

namespace Zero
{
	FDX12IndexBuffer::FDX12IndexBuffer(uint32_t* Indices, uint32_t Count)
		: IIndexBuffer(Indices, Count)
	{
	}

	void FDX12IndexBuffer::SetData(unsigned int* Indices, UINT32 Count)
	{
	}
}