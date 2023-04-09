
#include "DX12CommandQueue.h"
#include "DX12Device.h"
#include "DX12CommandList.h"

namespace Zero
{
	FDX12CommandQueue::FDX12CommandQueue(D3D12_COMMAND_LIST_TYPE Type)
		: m_CommandListType(Type)
		, m_FenceValue(0)
		, m_bProcessInFlightCommandLists(true)
	{
		auto D3DDevice = FDX12Device::Get()->GetDevice();
		
		D3D12_COMMAND_QUEUE_DESC Desc = {};
		Desc.Type = m_CommandListType;
		Desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		Desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		Desc.NodeMask = 0;
		ThrowIfFailed(D3DDevice->CreateCommandQueue(&Desc, IID_PPV_ARGS(&m_D3DCommandQueue)));
		ThrowIfFailed(D3DDevice->CreateFence(m_FenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence)));
		
		char ThreadName[256];
		sprintf_s(ThreadName, "ProccessInFlightCommandLists ");
		switch (m_CommandListType)
		{
		case D3D12_COMMAND_LIST_TYPE_COPY:
			m_D3DCommandQueue->SetName(L"Copy Command Queue");
			strcat_s(ThreadName, "(Copy)");
			break;
		case D3D12_COMMAND_LIST_TYPE_COMPUTE:
			m_D3DCommandQueue->SetName(L"Compute Command Queue");
			strcat_s(ThreadName, "(Compute)");
			break;
		case D3D12_COMMAND_LIST_TYPE_DIRECT:
			m_D3DCommandQueue->SetName(L"Direct Command Queue");
			strcat_s(ThreadName, "(Direct)");
			break;
		}

		m_ProcessInFlightCommandListsThread = std::thread(&FDX12CommandQueue::ProcessInFlightCommandLists, this);
		//Utils::SetThreadName(m_ProcessInFlightCommandListsThread, ThreadName);
	}
	FDX12CommandQueue::~FDX12CommandQueue()
	{
		m_bProcessInFlightCommandLists = false;
		m_ProcessInFlightCommandListsThread.join();
	}

	Ref<FDX12CommandList> FDX12CommandQueue::CreateNewCommandList()
	{
		auto CommandList = CreateRef<FDX12CommandList>(m_CommandListType);
		CommandList->Init();
		return CommandList;
	}

	Ref<FDX12CommandList> FDX12CommandQueue::GetCommandList()
	{
		Ref<FDX12CommandList> CommandList;
		
		if (!m_AvailableCommandLists.IsEmpty())
		{
			m_AvailableCommandLists.TryPop(CommandList);
		}
		else
		{
			CommandList = CreateNewCommandList();
		}
		CommandList->OnDeployed();
		return CommandList;
	}

	uint64_t FDX12CommandQueue::ExecuteCommandList(Ref<FDX12CommandList> CommandList)
	{
		return ExecuteCommandLists(std::vector<std::shared_ptr<FDX12CommandList>>({ CommandList }));
	}

	uint64_t FDX12CommandQueue::ExecuteCommandLists(const std::vector<Ref<FDX12CommandList>>& CommandLists)
	{
		FResourceStateTracker::Lock();
		
		// Command lists that need to put back on the command list queue.
		std::vector<Ref<FDX12CommandList>> ToBeQueued;
		ToBeQueued.reserve(CommandLists.size() * 2);
		
		// Command lists that need to be executed.
		std::vector<ID3D12CommandList*> D3DCommandLists;
		D3DCommandLists.reserve(CommandLists.size() * 2);
		
		for (auto CommandList : CommandLists)
		{
			auto PendingCommandList = GetCommandList();
			bool bHasPendingBarriers = CommandList->Close(PendingCommandList);
			PendingCommandList->Close();
			// If there are no pending barriers on the pending command list, there is no reason to
			// execute an empty command list on the command queue.
			if (bHasPendingBarriers)
			{
				D3DCommandLists.push_back(PendingCommandList->GetD3D12CommandList().Get());
			}
			D3DCommandLists.push_back(CommandList->GetD3D12CommandList().Get());
			
			ToBeQueued.push_back(PendingCommandList);
			ToBeQueued.push_back(CommandList);
		}
		
		UINT NumCommandLists = static_cast<UINT>(D3DCommandLists.size());
		m_D3DCommandQueue->ExecuteCommandLists(NumCommandLists, D3DCommandLists.data());
		
		uint64_t FenceValue = Signal();

		FResourceStateTracker::Unlock();
		
		for (auto CommandList : ToBeQueued)
		{
			m_InFlightCommandLists.Push({ FenceValue, CommandList});
		}

		return FenceValue;
	}

	uint64_t FDX12CommandQueue::ExecuteCommandList_Raw(Ref<FDX12CommandList> CommandList)
	{
		return ExecuteCommandLists_Raw(std::vector<std::shared_ptr<FDX12CommandList>>({ CommandList }));
	}

	uint64_t FDX12CommandQueue::ExecuteCommandLists_Raw(const std::vector<Ref<FDX12CommandList>>& CommandLists)
	{
		UINT NumCommandLists = static_cast<UINT>(CommandLists.size());
		
		std::vector<ID3D12CommandList*> D3DCommandLists;
		for (auto CommandList : CommandLists)
		{
			D3DCommandLists.push_back(CommandList->GetD3D12CommandList().Get());
		}
		m_D3DCommandQueue->ExecuteCommandLists(NumCommandLists, D3DCommandLists.data());
		
		uint64_t FenceValue = Signal();
		
		for (auto CommandList : CommandLists)
		{
			m_InFlightCommandLists.Push({ FenceValue, CommandList });
		}

		return FenceValue;
	}

	uint64_t FDX12CommandQueue::Signal()
	{
		uint64_t OutFenceValue = ++m_FenceValue;
		m_D3DCommandQueue->Signal(m_Fence.Get(), m_FenceValue);
		return OutFenceValue;
	}
	bool FDX12CommandQueue::IsFenceComplete(uint64_t FenceValue)
	{
		return m_Fence->GetCompletedValue() >= FenceValue;
	}
	void FDX12CommandQueue::WaitForFenceValue(uint64_t FenceValue)
	{
		if (!IsFenceComplete(FenceValue))
		{
			auto Event = ::CreateEvent(NULL, FALSE, FALSE, NULL);
			if (Event)
			{
				// Is this function thread safe?
				m_Fence->SetEventOnCompletion(FenceValue, Event);
				::WaitForSingleObject(Event, DWORD_MAX);

				::CloseHandle(Event);
			}
		}
	}
	void FDX12CommandQueue::Flush()
	{
		std::unique_lock<std::mutex> Lock(m_ProcessInFlightCommandListsThreadMutex);
		m_ProcessInFlightCommandListsThreadCV.wait(Lock, [this] { return m_InFlightCommandLists.IsEmpty(); });

		WaitForFenceValue(m_FenceValue);
	}

	void FDX12CommandQueue::Wait(const FDX12CommandQueue& Other)
	{
		m_D3DCommandQueue->Wait(m_Fence.Get(), Other.m_FenceValue);
	}

	void FDX12CommandQueue::ProcessInFlightCommandLists()
	{
		std::unique_lock<std::mutex> Lock(m_ProcessInFlightCommandListsThreadMutex, std::defer_lock);

		while (m_bProcessInFlightCommandLists)
		{
			FCommandListEntry CommandListEntry;

			Lock.lock();
			while (m_InFlightCommandLists.TryPop(CommandListEntry))
			{
				auto FenceValue = std::get<0>(CommandListEntry);
				auto CommandList = std::get<1>(CommandListEntry);

				WaitForFenceValue(FenceValue);

				CommandList->Reset();
				m_AvailableCommandLists.Push(CommandList);
			}
			Lock.unlock();
			m_ProcessInFlightCommandListsThreadCV.notify_one();

			// std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
			std::this_thread::yield();
		}
	}
}