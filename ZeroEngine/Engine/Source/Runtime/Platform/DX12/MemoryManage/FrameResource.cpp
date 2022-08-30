#include "FrameResource.h"
#include "../DX12Device.h"
#include "UploadBuffer.h"

namespace Zero
{
	FFrameResourcesManager& FFrameResourcesManager::GetInstance()
	{
		static FFrameResourcesManager instance;
		return instance;
		// TODO: 在此处插入 return 语句
	}
	FFrameResourcesManager::FFrameResourcesManager(int FrameResourcesCount)
		: m_FrameResourcesCount(FrameResourcesCount)
		, m_CurrentFrameIndex(0)
	{
	}
	void FFrameResourcesManager::Init(Ref<FDX12Device> Device)
	{
		m_Device = Device;
		m_Fences.reserve(m_FrameResourcesCount);
		m_CommandAllocators.reserve(m_FrameResourcesCount);
		m_UploadBuffers.reserve(m_FrameResourcesCount);
		
		for (size_t i = 0; i < m_FrameResourcesCount; ++i)
		{
			m_Fences[i] = 0;
			ThrowIfFailed(
				m_Device->GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CommandAllocators[i]))
			);
			m_UploadBuffers[i].reset(new FUploadBuffer(*m_Device.get()));
		}
	}
	FFrameResourcesManager::~FFrameResourcesManager()
	{
		for (int i = 0; i < m_FrameResourcesCount; i++)
		{
			m_UploadBuffers[i] = nullptr;
			m_CommandAllocators[i] = nullptr;
			m_Fences[i] = 0;
		}
		m_UploadBuffers.clear();
		m_Fences.clear();
		m_CommandAllocators.clear();
	}
	void FFrameResourcesManager::SetCurrentFence(uint64_t FenceValue)
	{
		m_Fences[m_CurrentFrameIndex] = FenceValue;
	}
}
