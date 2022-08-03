#pragma once

#include "Core.h"
#include "Common/DX12Headder.h"
#include "Render/RHI/CommandList.h"


namespace Zero
{
	class FDX12CommandList : public FCommandList
	{
	public:
		FDX12CommandList(D3D12_COMMAND_LIST_TYPE Type);
		virtual ~FDX12CommandList() = default;
	private:
		D3D12_COMMAND_LIST_TYPE  CommandListType;
		ComPtr<ID3D12CommandAllocator>	CommandAllocator;
		ComPtr<ID3D12GraphicsCommandList> CommandList;


		virtual void Reset();
		virtual void Execute();
	}
}