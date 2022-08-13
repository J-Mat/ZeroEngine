
#include "DX12CommandQueue.h"

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
		ThrowIfFailed(D3DDevice->CreateCommandQueue(&Desc, IID_PPV_ARGS(&CommandQueue)));
		ThrowIfFailed(D3DDevice->CreateFence(m_FenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&Fence)));
		
		char ThreadName[256];
		sprintf_s(ThreadName, "ProccessInFlightCommandLists ");
		switch (m_CommandListType)
		{
		case D3D12_COMMAND_LIST_TYPE_COPY:
			CommandQueue->SetName(L"Copy Command Queue");
			strcat_s(ThreadName, "(Copy)");
			break;
		case D3D12_COMMAND_LIST_TYPE_COMPUTE:
			CommandQueue->SetName(L"Compute Command Queue");
			strcat_s(ThreadName, "(Compute)");
			break;
		case D3D12_COMMAND_LIST_TYPE_DIRECT:
			CommandQueue->SetName(L"Direct Command Queue");
			strcat_s(ThreadName, "(Direct)");
			break;
		}

		ProcessInFlightCommandListsThread = std::thread(&FDX12CommandQueue::ProcessInFlightCommandLists, this);
		Utils::SetThreadName(ProcessInFlightCommandListsThread, ThreadName);
	}
	FDX12CommandQueue::~FDX12CommandQueue()
	{
		m_bProcessInFlightCommandLists = false;
		ProcessInFlightCommandListsThread.join();
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
		return uint64_t();
	}

	uint64_t FDX12CommandQueue::Signal()
	{
		uint64_t OutFenceValue = ++m_FenceValue;
		CommandQueue->Signal(Fence.Get(), m_FenceValue);
		return OutFenceValue;
	}
	bool FDX12CommandQueue::IsFenceComplete(uint64_t FenceValue)
	{
		Fence->GetCompletedValue() >= FenceValue;
	}
	void FDX12CommandQueue::WaitForFenceValue(uint64_t FenceValue)
	{
		if (!IsFenceComplete(FenceValue))
		{
			auto Event = ::CreateEvent(NULL, FALSE, FALSE, NULL);
			if (Event)
			{
				// Is this function thread safe?
				Fence->SetEventOnCompletion(FenceValue, Event);
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