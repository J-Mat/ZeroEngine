#include "Buffer.h"

namespace Zero
{

	FBuffer::FBuffer(FDX12Device& Device, const D3D12_RESOURCE_DESC& ResDesc)
		:FResource(Device, ResDesc)
	{
	}

	FBuffer::FBuffer(FDX12Device& Device, Microsoft::WRL::ComPtr<ID3D12Resource> Resource)
		:FResource(Device, Resource)
	{
	}
}
