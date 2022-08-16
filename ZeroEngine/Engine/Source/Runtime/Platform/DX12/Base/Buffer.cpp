#include "Buffer.h"

namespace Zero
{
	IBuffer::IBuffer(FDX12Device& m_Device)
		:IResource(m_Device)
	{
	}
	IBuffer::IBuffer(FDX12Device& m_Device, const D3D12_RESOURCE_DESC& ResDesc)
		:IResource(m_Device, ResDesc)
	{
	}

	IBuffer::IBuffer(FDX12Device& m_Device, Microsoft::WRL::ComPtr<ID3D12Resource> Resource)
		:IResource(m_Device, Resource)
	{
	}
}
