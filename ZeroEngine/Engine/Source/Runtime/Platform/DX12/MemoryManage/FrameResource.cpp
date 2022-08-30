#include "FrameResource.h"
#include "../DX12Device.h"
#include "UploadBuffer.h"

namespace Zero
{
	FFrameResourceBuffer::FFrameResourceBuffer(uint32_t Size)
		: m_SizeByte(Size)
	{
		FFrameResourcesManager::GetInstance().RegistFrameResource(this);
	}

	inline D3D12_GPU_VIRTUAL_ADDRESS FFrameResourceBuffer::GetCurrentGPUAddress()
	{
		uint32_t Index = FFrameResourcesManager::GetInstance().GetCurrentIndex();
		memcpy(m_GPUBuffers[Index].CPU
			, m_CPUBuffer
			, m_SizeByte);
	}

	void FFrameResourceBuffer::UploadCurrentBuffer()
	{
		uint32_t Index = FFrameResourcesManager::GetInstance().GetCurrentIndex();
		memcpy(m_GPUBuffers[Index].CPU
			, m_CPUBuffer
			, m_SizeByte);
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
