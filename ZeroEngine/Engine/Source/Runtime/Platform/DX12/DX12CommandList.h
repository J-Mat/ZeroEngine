#pragma once

#include "Core.h"
#include "Common/DX12Header.h"
#include "Render/RHI/CommandList.h"
#include "DX12Device.h"


namespace Zero
{
	class FDX12CommandList : public FCommandList
	{
	public:
		FDX12CommandList(FDX12Device& InDevice, D3D12_COMMAND_LIST_TYPE Type);
		virtual ~FDX12CommandList() = default;
		FDX12Device& GetDevice() const
    	{
        	return Device;
    	}

		void FlushResourceBarriers();


		/**
     	* Draw geometry.
     	*/
    	void Draw( uint32_t vertexCount, uint32_t instanceCount = 1, uint32_t startVertex = 0, uint32_t startInstance = 0 );
    	void DrawIndexed( uint32_t indexCount, uint32_t instanceCount = 1, uint32_t startIndex = 0, int32_t baseVertex = 0,
                      uint32_t startInstance = 0 );

	private:
		FDX12Device& Device;
		D3D12_COMMAND_LIST_TYPE  CommandListType;
		ComPtr<ID3D12CommandAllocator>	CommandAllocator;
		ComPtr<ID3D12GraphicsCommandList> CommandList;


		virtual void Reset();
		virtual void Execute();
	}
}