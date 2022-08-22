
#include "DX12CommandQueue.h"
#include "DX12Device.h"
#include "DX12CommandList.h"

namespace Zero
{
	FDX12CommandQueue::FDX12CommandQueue(FDX12Device& InDevice, D3D12_COMMAND_LIST_TYPE Type)
		: m_Device(InDevice)
		, m_CommandListType(Type)
		, m_FenceValue(0)
		, m_bProcessInFlightCommandLists(true)
	{
		auto D3DDevice = m_Device.GetDevice();
		
		D3D12_COMMAND_QUEUE_DESC Desc = {};
		Desc.Type = m_CommandListType;
		Desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		Desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		Desc.NodeMask = 0;
		ThrowIfFailed(D3DDevice->CreateCommandQueue(&Desc, IID_PPV_ARGS(&m_CommandQueue)));
		ThrowIfFailed(D3DDevice->CreateFence(m_FenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence)));
		
		char ThreadName[256];
		sprintf_s(ThreadName, "ProccessInFlightCommandLists ");
		switch (m_CommandListType)
		{
		case D3D12_COMMAND_LIST_TYPE_COPY:
			m_CommandQueue->SetName(L"Copy Command Queue");
			strcat_s(ThreadName, "(Copy)");
			break;
		case D3D12_COMMAND_LIST_TYPE_COMPUTE:
			m_CommandQueue->SetName(L"Compute Command Queue");
			strcat_s(ThreadName, "(Compute)");
			break;
		case D3D12_COMMAND_LIST_TYPE_DIRECT:
			m_CommandQueue->SetName(L"Direct Command Queue");
			strcat_s(ThreadName, "(Direct)");
			break;
		}

		m_ProcessInFlightCommandListsThread = std::thread(&FDX12CommandQueue::ProcessInFlightCommandLists, this);
		Utils::SetThreadName(m_ProcessInFlightCommandListsThread, ThreadName);
	}
	FDX12CommandQueue::~FDX12CommandQueue()
	{
		m_bProcessInFlightCommandLists = false;
		m_ProcessInFlightCommandListsThread.join();
	}

	Ref<FDX12CommandList> FDX12CommandQueue::GetCommandList()
	{
		Ref<FDX12CommandList> CommandList;
		
		if (AvailableCommandLists.IsEmpty())
		{
			AvailableCommandLists.TryPop(CommandList);
		}
		else
		{
			CommandList = CreateRef<FDX12CommandList>(m_Device, m_CommandListType);
		}
		
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
		return 0;
	
	}

	uint64_t FDX12CommandQueue::Signal()
	{
		uint64_t OutFenceValue = ++m_FenceValue;
		m_CommandQueue->Signal(m_Fence.Get(), m_FenceValue);
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
		std::unique_lock<std::mutex> Lock(ProcessInFlightCommandListsThreadMutex);
		ProcessInFlightCommandListsThreadCV.wait(Lock, [this] { return InFlightCommandLists.IsEmpty(); });

		WaitForFenceValue(m_FenceValue);
	}

	void FDX12CommandQueue::ProcessInFlightCommandLists()
	{
	}
}