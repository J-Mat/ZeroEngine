#include "DX12CommandList.h"

namespace Zero
{
	FDX12CommandList::FDX12CommandList(DX12Device& InDevice, D3D12_COMMAND_LIST_TYPE Type)
	: Device(InDevice), 
	CommandListType(Type)
	{
		ThrowIfFailed(Device->CreateCommandAllocator(CommandListType, IID_PPV_ARGS(&CommandAllocator)));	

		ThrowIfFailed(Device->CreateCommandList(0, CommandListType, CommandAllocator.Get(),
			nullptr, IID_PPV_ARGS(&CommandList)));
		
	}
	
	void FDX12CommandList::FlushResourceBarriers()
	{
		
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
		ThrowIfFailed(m_GraphicCmdList->Close());
		ID3D12CommandList* cmdLists[] = { m_GraphicCmdList.Get() };
		DX12Context::GetCommandQueue()->ExecuteCommandLists(_countof(cmdLists), cmdLists);
		m_IsClosed = true;
	}
}
