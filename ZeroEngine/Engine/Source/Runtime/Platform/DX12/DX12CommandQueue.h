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
		FDX12CommandQueue(FDX12Device& InDevice, D3D12_COMMAND_LIST_TYPE Type);
		virtual ~FDX12CommandQueue();

		Ref<FDX12CommandList> GetCommandList();
	
		ComPtr<ID3D12CommandQueue> GetD3DCommandQueue() { return m_CommandQueue; }


		uint64_t ExecuteCommandList(Ref<FDX12CommandList> CommandList);
		uint64_t ExecuteCommandLists(const std::vector<Ref<FDX12CommandList>>& CommandLists);

		uint64_t Signal();
		bool     IsFenceComplete(uint64_t fenceValue);
		void     WaitForFenceValue(uint64_t fenceValue);
		void     Flush();

	private:
		void ProcessInFlightCommandLists();
		
		using CommandListEntry = std::tuple<uint64_t, Ref<FDX12CommandList>>;
		
		FDX12Device& m_Device;
		D3D12_COMMAND_LIST_TYPE                    m_CommandListType;
		ComPtr<ID3D12CommandQueue> m_CommandQueue;
		ComPtr<ID3D12Fence>        m_Fence;
		std::atomic_uint64_t                       m_FenceValue;
		
		TThreadSafeQueue<CommandListEntry> InFlightCommandLists;
		TThreadSafeQueue<Ref<FDX12CommandList>> AvailableCommandLists;
		
		std::thread             m_ProcessInFlightCommandListsThread;
		std::atomic_bool        m_bProcessInFlightCommandLists;
		std::mutex              ProcessInFlightCommandListsThreadMutex;
		std::condition_variable ProcessInFlightCommandListsThreadCV;
	};
}