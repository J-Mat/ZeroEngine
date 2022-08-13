#include "Buffer.h"

namespace Zero
{
	IBuffer::IBuffer(FDX12Device& Device)
		:IResource(Device)
	{
	}
	IBuffer::IBuffer(FDX12Device& Device, const D3D12_RESOURCE_DESC& ResDesc)
		:IResource(Device, ResDesc)
	{
	}

	IBuffer::IBuffer(FDX12Device& Device, Microsoft::WRL::ComPtr<ID3D12Resource> Resource)
		:IResource(Device, Resource)
	{
	}
}
