#include "DX12CommandList.h"

namespace Zero
{
	FDX12CommandList::FDX12CommandList(FDX12Device& InDevice, D3D12_COMMAND_LIST_TYPE Type)
	: m_Device(InDevice), 
	m_CommandListType(Type)
	{
		ThrowIfFailed(m_Device.GetDevice()->CreateCommandAllocator(m_CommandListType, IID_PPV_ARGS(&m_CommandAllocator)));

		ThrowIfFailed(m_Device.GetDevice()->CreateCommandList(0, m_CommandListType, m_CommandAllocator.Get(),
			nullptr, IID_PPV_ARGS(&m_CommandList)));
	
		m_UploadBuffer = CreateScope<FUploadBuffer>(m_Device);

		m_ResourceStateTracker = CreateScope<FResourceStateTracker>();

		for (int i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i)
		{
			m_DescriptorHeaps[i] = nullptr;
		}
	}

	void FDX12CommandList::FlushResourceBarriers()
	{
		
	}

	ComPtr<ID3D12Resource> FDX12CommandList::CreateDefaultBuffer(const void* bufferData, size_t bufferSize, D3D12_RESOURCE_FLAGS flags)
	{
		ComPtr<ID3D12Resource> D3DResource;

		return ComPtr<ID3D12Resource>();
	}
	

	void FDX12CommandList::Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t startVertex, uint32_t startInstance)
	{
		
	}
	
	void FDX12CommandList::DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t startIndex, int32_t baseVertex,
						  uint32_t startInstance)
	{
		
	}
			
	void FDX12CommandList::Reset()
	{
		
	}
	
	void FDX12CommandList::Execute()
	{
		DX12Context::GetCommandQueue()->ExecuteCommandLists(_countof(cmdLists), cmdLists);
		m_IsClosed = true;
	}
}
