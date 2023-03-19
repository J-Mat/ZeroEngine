#include "FrameResource.h"
#include "../../DX12Device.h"
#include "UploadBuffer.h"
#include "Utils.h"

namespace Zero
{
	FFrameResourceBuffer::FFrameResourceBuffer(uint32_t Size, bool bDynamic)
		: m_SizeByte(ZMath::CalcConstantBufferByteSize(Size))
		, m_bDynamic(bDynamic)
	{
		if (!m_bDynamic)
		{
			FFrameResourcesManager::Get().RegistFrameResource(this);
		}
		else
		{
			m_AddressBuffers.resize(FFrameResourcesManager::Get().GetResourceCount());
		}
	}

	void FFrameResourceBuffer::AllocateDynamicSpace()
	{
		CORE_ASSERT(m_bDynamic, "Constant Buffer must be dynamic")
		uint32_t Index = FFrameResourcesManager::Get().GetCurrentIndex();
		FDynamicAllocation DynamicAllocation =  FDX12Device::Get()->GetLinearDynamicAllocator()->Allocate(m_SizeByte);
		m_AddressBuffers[Index].CPU = DynamicAllocation.CpuAddress;
		m_AddressBuffers[Index].GPU = DynamicAllocation.GpuAddress;
	}

	void FFrameResourceBuffer::CopyDataToConstantsBuffer(void* Data, uint32_t Offset, uint32_t Length)
	{
		uint32_t Index = FFrameResourcesManager::Get().GetCurrentIndex();
		void* Target = (void*)((char*)m_AddressBuffers[Index].CPU + Offset);
		memcpy(Target, Data, Length);
	}

	void FFrameResourceBuffer::CopyDataFromConstantsBuffer(void* Data, uint32_t Offset, uint32_t Length)
	{
		uint32_t Index = FFrameResourcesManager::Get().GetCurrentIndex();
		void* ConstBuffer = (void*)((char*)m_AddressBuffers[Index].CPU + Offset);
		memcpy(Data, ConstBuffer, Length);
	}

	void* FFrameResourceBuffer::GetPtrFromConstantsBuffer(uint32_t Offset)
	{
		uint32_t Index = FFrameResourcesManager::Get().GetCurrentIndex();
		return (void*)((char*)m_AddressBuffers[Index].CPU + Offset);
	}

	D3D12_GPU_VIRTUAL_ADDRESS FFrameResourceBuffer::GetCurrentGPUAddress()
	{
		uint32_t Index = FFrameResourcesManager::Get().GetCurrentIndex();
		/*
		memcpy(m_AddressBuffers[Index].CPU
			, m_CPUBuffer
			, m_SizeByte);
		*/
		return m_AddressBuffers[Index].GPU;
	}

	void* FFrameResourceBuffer::GetBuffer()
	{
		uint32_t Index = FFrameResourcesManager::Get().GetCurrentIndex();
		return m_AddressBuffers[Index].CPU;
	}

	void FFrameResourceBuffer::UploadCurrentBuffer()
	{
		return;
		/*
		uint32_t Index = FFrameResourcesManager::Get().GetCurrentIndex();
		memcpy(m_AddressBuffers[Index].CPU
			, m_CPUBuffer
			, m_SizeByte);
		*/
	}


	FFrameResourcesManager::FFrameResourcesManager(int FrameResourcesCount)
		: m_FrameResourcesCount(FrameResourcesCount)
		, m_CurrentFrameIndex(0)
	{
	}
	void FFrameResourcesManager::Init()
	{
		m_Fences.resize(m_FrameResourcesCount);
		m_CommandAllocators.resize(m_FrameResourcesCount);
		m_UploadBuffers.resize(m_FrameResourcesCount);
		
		for (size_t i = 0; i < m_FrameResourcesCount; ++i)
		{
			m_Fences[i] = 0;
			ThrowIfFailed(
				FDX12Device::Get()->GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CommandAllocators[i]))
			);
			m_UploadBuffers[i].reset(new FUploadBuffer());
		}
	}
	FFrameResourcesManager::~FFrameResourcesManager()
	{
		for (uint32_t i = 0; i < m_FrameResourcesCount; i++)
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
