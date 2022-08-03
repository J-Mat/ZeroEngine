#pragma once

#include "Core.h"
#include "Common/DX12Header.h"
#include "DX12CommandList.h"


namespace Zero
{
	class DX12CommandQueue	
	{
	public:
		DX12CommandQueue(D3D12_COMMAND_LIST_TYPE Type);
		virtual ~DX12CommandQueue() = default;
		
		Ref<FDX12CommandList> GetCommandList();
	}
}