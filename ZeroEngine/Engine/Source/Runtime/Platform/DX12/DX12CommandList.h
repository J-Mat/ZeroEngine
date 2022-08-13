#pragma once

#include "Core.h"
#include "Common/DX12Header.h"
#include "Render/RHI/CommandList.h"
#include "GPUMemory/UploadBuffer.h"
#include "GPUMemory/ResourceStateTracker.h"
#include "DX12Device.h"


namespace Zero
{
	class FDX12CommandList : public FCommandList
	{
	public:
		FDX12CommandList(FDX12Device& InDevice, D3D12_COMMAND_LIST_TYPE Type);
		virtual ~FDX12CommandList() = default;

		void FlushResourceBarriers();
		ComPtr<ID3D12GraphicsCommandList2> GetD3D12CommandList() const { return m_CommandList;}

		ComPtr<ID3D12Resource> CreateDefaultBuffer(const void* BufferData, size_t BufferSize, D3D12_RESOURCE_FLAGS Flags = D3D12_RESOURCE_FLAG_NONE);
		/**
		* Draw geometry.
		*/
		void Draw(uint32_t vertexCount, uint32_t instanceCount = 1, uint32_t startVertex = 0, uint32_t startInstance = 0);
		void DrawIndexed(uint32_t indexCount, uint32_t instanceCount = 1, uint32_t startIndex = 0, int32_t baseVertex = 0,
			uint32_t startInstance = 0);

	private:
		FDX12Device& m_Device;
		D3D12_COMMAND_LIST_TYPE  m_CommandListType;
		ComPtr<ID3D12CommandAllocator>	m_CommandAllocator;
		ComPtr<ID3D12GraphicsCommandList2> m_CommandList;
		Scope<FUploadBuffer> m_UploadBuffer;
		Scope<FResourceStateTracker> m_ResourceStateTracker;
		ID3D12DescriptorHeap* m_DescriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];


		virtual void Reset();
		virtual void Execute();
	};
}