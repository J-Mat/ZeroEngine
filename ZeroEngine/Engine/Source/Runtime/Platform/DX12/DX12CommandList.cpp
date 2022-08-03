#include "DX12CommandList.h"
#include "DX12Content.h"

namespace Zero
{
	FDX12CommandList::FDX12CommandList(D3D12_COMMAND_LIST_TYPE Type) 
	: CommandListType(Type)
	{
		auto* Device = FDX12Content::GetInstance().GetDevice();

		ThrowIfFailed(Device->CreateCommandAllocator(CommandListType, IID_PPV_ARGS(&CommandAllocator)));	

		ThrowIfFailed(Device->CreateCommandList(0, CommandListType, CommandAllocator.Get(),
			nullptr, IID_PPV_ARGS(&CommandList)));
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
