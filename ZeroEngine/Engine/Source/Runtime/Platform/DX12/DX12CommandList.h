#pragma once

#include "Core.h"
#include "Common/DX12Header.h"
#include "Render/RHI/CommandList.h"
#include "GPUMemory/UploadBuffer.h"
#include "GPUMemory/ResourceStateTracker.h"
#include "DX12Device.h"
#include "Render/Moudule/Image/Image.h"



namespace Zero
{
	class FDX12CommandList : public FCommandList, public std::enable_shared_from_this<FDX12CommandList>
	{
	public:
		FDX12CommandList(FDX12Device& InDevice, D3D12_COMMAND_LIST_TYPE Type);
		virtual ~FDX12CommandList() = default;

		void FlushResourceBarriers();
		ComPtr<ID3D12GraphicsCommandList2> GetD3D12CommandList() const { return m_D3DCommandList;}

		ComPtr<ID3D12Resource> CreateDefaultBuffer(const void* BufferData, size_t BufferSize, D3D12_RESOURCE_FLAGS Flags = D3D12_RESOURCE_FLAG_NONE);
		

		ComPtr<ID3D12Resource> CreateTextureResource(Ref<FImage> Image);

		/**
		* Draw geometry.
		*/
		void Draw(uint32_t vertexCount, uint32_t instanceCount = 1, uint32_t startVertex = 0, uint32_t startInstance = 0);
		void DrawIndexed(uint32_t indexCount, uint32_t instanceCount = 1, uint32_t startIndex = 0, int32_t baseVertex = 0,
			uint32_t startInstance = 0);
		
		/**
		 * Close the command list.
		* Used by the command queue.
		*
		* @param pendingCommandList The command list that is used to execute pending
		* resource barriers (if any) for this command list.
		*
		* @return true if there are any pending resource barriers that need to be
		* processed.
		 */
		bool Close(const Ref<FDX12CommandList>& PendingCommandList);
		// Just close the command list. This is useful for pending command lists.
		void Close();

		/**
		* Reset the command list. This should only be called by the CommandQueue
		* before the command list is returned from CommandQueue::GetCommandList.
		*/
		void Reset();
		
		/**
		* Release tracked objects. Useful if the swap chain needs to be resized.
		*/
		void ReleaseTrackedObjects();

		/**
		* Set the currently bound descriptor heap.
		* Should only be called by the DynamicDescriptorHeap class.
		*/
		void SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE HeapType, ID3D12DescriptorHeap* Heap);

		// Binds the current descriptor heaps to the command list.
		void BindDescriptorHeaps();


		/**
		* Transition a resource to a particular state.
		*
		* @param resource The resource to transition.
		* @param stateAfter The state to transition the resource to. The before state is resolved by the resource state
		* tracker.
		* @param subresource The subresource to transition. By default, this is D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES
		* which indicates that all subresources are transitioned to the same state.
		* @param flushBarriers Force flush any barriers. Resource barriers need to be flushed before a command (draw,
		* dispatch, or copy) that expects the resource to be in a particular state can run.
		*/
		void TransitionBarrier(const Ref<IResource>& Resource, D3D12_RESOURCE_STATES StateAfter,
			UINT Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, bool bFlushBarriers = false);
		void TransitionBarrier(ComPtr<ID3D12Resource> Resource, D3D12_RESOURCE_STATES StateAfter,
			UINT Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, bool bFlushBarriers = false);

	private:
		void TrackResource(Microsoft::WRL::ComPtr<ID3D12Object> Object);
		void TrackResource(const Ref<IResource>& res);

	private:
		std::vector<ComPtr<ID3D12Object>> m_TrackedObjects;
		FDX12Device& m_Device;
		D3D12_COMMAND_LIST_TYPE  m_CommandListType;
		ComPtr<ID3D12CommandAllocator>	m_CommandAllocator;
		ComPtr<ID3D12GraphicsCommandList2> m_D3DCommandList;
		Scope<FUploadBuffer> m_UploadBuffer;
		Scope<FResourceStateTracker> m_ResourceStateTracker;
		ID3D12DescriptorHeap* m_DescriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];


		virtual void Reset();
		virtual void Execute();
	};
}