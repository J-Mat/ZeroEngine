
#include "DX12CommandQueue.h"

namespace Zero
{
	FDX12CommandQueue::FDX12CommandQueue(FDX12Device& InDevice, D3D12_COMMAND_LIST_TYPE Type)
		: Device(InDevice)
		, CommandListType(Type)
		, FenceValue(0)
		, bProcessInFlightCommandLists(true)
	{
		auto D3DDevice = Device.GetDevice();
		
		D3D12_COMMAND_QUEUE_DESC Desc = {};
		Desc.Type = CommandListType;
		Desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		Desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		Desc.NodeMask = 0;
		ThrowIfFailed(D3DDevice->CreateCommandQueue(&Desc, IID_PPV_ARGS(&CommandQueue)));
		ThrowIfFailed(D3DDevice->CreateFence(FenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&Fence)));
		
		char ThreadName[256];
		sprintf_s(ThreadName, "ProccessInFlightCommandLists ");
		switch (CommandListType)
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
		bProcessInFlightCommandLists = false;
		ProcessInFlightCommandListsThread.join();
	}
	Ref<FDX12CommandList> FDX12CommandQueue::GetCommandList()
	{
		return Ref<FDX12CommandList>();
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
		uint64_t OutFenceValue = ++FenceValue;
		CommandQueue->Signal(Fence.Get(), FenceValue);
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
	}
	void FDX12CommandQueue::ProcessInFlightCommandLists()
	{
	}
}