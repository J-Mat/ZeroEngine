#pragma once

#include "Core.h"
#include "Common/DX12Header.h"
#include "Core/Base/ThreadSafeQueue.h"


namespace Zero
{
	class FDX12Device;
	class FDX12CommandList; 
	class FDX12CommandQueue
	{
	public:
		FDX12CommandQueue(D3D12_COMMAND_LIST_TYPE Type);
		virtual ~FDX12CommandQueue();

		Ref<FDX12CommandList> CreateNewCommandList();
		Ref<FDX12CommandList> GetCommandList();
	
		ComPtr<ID3D12CommandQueue> GetD3DCommandQueue() { return m_D3DCommandQueue; }


		uint64_t ExecuteCommandList(Ref<FDX12CommandList> CommandList);
		uint64_t ExecuteCommandLists(const std::vector<Ref<FDX12CommandList>>& CommandLists);

		uint64_t Signal();
		bool     IsFenceComplete(uint64_t fenceValue);
		void     WaitForFenceValue(uint64_t fenceValue);
		void     Flush();

		void Wait(const FDX12CommandQueue& Other);

	private:
		void ProcessInFlightCommandLists();
		

		// Keep track of command allocators that are "in-flight"
		// The first member is the fence value to wait for, the second is the
		// a shared pointer to the "in-flight" command list.
		using FCommandListEntry = std::tuple<uint64_t, Ref<FDX12CommandList>>;
		

		D3D12_COMMAND_LIST_TYPE                    m_CommandListType;
		ComPtr<ID3D12CommandQueue> m_D3DCommandQueue;
		ComPtr<ID3D12Fence>        m_Fence;
		std::atomic_uint64_t                       m_FenceValue;
		
		TThreadSafeQueue<FCommandListEntry> m_InFlightCommandLists;
		TThreadSafeQueue<Ref<FDX12CommandList>> m_AvailableCommandLists;
		
		std::thread             m_ProcessInFlightCommandListsThread;
		std::atomic_bool        m_bProcessInFlightCommandLists;
		std::mutex              m_ProcessInFlightCommandListsThreadMutex;
		std::condition_variable m_ProcessInFlightCommandListsThreadCV;
	};
}