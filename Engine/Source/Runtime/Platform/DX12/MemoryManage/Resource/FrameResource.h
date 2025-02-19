#pragma once

#include "Core.h"
#include "../../Common/DX12Header.h"
#include "Core/Base/PublicSingleton.h"
#include "UploadBuffer.h"
#include "FrameResource.h"



namespace Zero
{
	class FFrameResourcesManager;
	class FDX12Device;
	class FUploadBuffer;

	template<typename T>
	class FDX12FrameResource
	{
	public:
		friend class FFrameResourcesManager;
		FDX12FrameResource();

		Ref<T> GetCurrentBuffer();
		void UploadCurrentBuffer();

		D3D12_GPU_VIRTUAL_ADDRESS GetCurrentGPUAddress();
	private:
		std::vector<FUploadBuffer::FAllocation> m_GPUBuffers;
		std::vector<Ref<T>> m_CPUBuffers;
	};

	class FFrameResourceBuffer
	{
	public:
		friend class FFrameResourcesManager;
		FFrameResourceBuffer(uint32_t Size, bool bDynamic);

		~FFrameResourceBuffer()
		{
		}
		void AllocateDynamicSpace();
		void CopyDataToConstantsBuffer(void* Data, uint32_t Offset, uint32_t Length);


		void CopyDataFromConstantsBuffer(void* Data, uint32_t Offset, uint32_t Length);

		void* GetPtrFromConstantsBuffer(uint32_t Offset);

		D3D12_GPU_VIRTUAL_ADDRESS GetCurrentGPUAddress();

		void* GetBuffer();
	

		void UploadCurrentBuffer();
		uint32_t GetSizeInByte() { return m_SizeByte; }
	private:
		std::vector<FUploadBuffer::FAllocation> m_AddressBuffers;
		uint32_t m_SizeByte;;
		bool m_bDynamic;
	};


	class IDevice;
	class FFrameResourcesManager : public IPublicSingleton<FFrameResourcesManager>
	{
	public:
		FFrameResourcesManager(int FrameResourcesCount = 3);
		virtual ~FFrameResourcesManager();
		void Init();
		void SetCurrentFence(uint64_t FenceValue);
		void UseNextFrameResource() { m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % m_FrameResourcesCount; }
		uint32_t GetCurrentIndex() { return m_CurrentFrameIndex; }
		uint32_t GetResourceCount() {return m_FrameResourcesCount; }


		template<typename T>
		void RegistFrameResource(FDX12FrameResource<T>* FrameResource)
		{
			FrameResource->m_CPUBuffers.resize(m_FrameResourcesCount);
			FrameResource->m_GPUBuffers.resize(m_FrameResourcesCount);
			for (int i = 0; i < m_FrameResourcesCount; ++i)
			{
				FrameResource->m_CPUBuffers[i].reset(new T);
				FrameResource->m_GPUBuffers[i] = m_UploadBuffers[i]->Allocate(sizeof(T), true);
			}
			new FUploadBuffer::FAllocation
		}

		void RegistFrameResource(FFrameResourceBuffer* FrameResourceBuffer)
		{
			//FrameResourceBuffer->m_CPUBuffer = new byte[FrameResourceBuffer->m_SizeByte];
			FrameResourceBuffer->m_AddressBuffers.resize(m_FrameResourcesCount);
			for (uint32_t i = 0; i < m_FrameResourcesCount; ++i)
			{

				FrameResourceBuffer->m_AddressBuffers[i] = m_UploadBuffers[i]->Allocate(FrameResourceBuffer->m_SizeByte, true);
			}
		}

	private:
		uint32_t m_FrameResourcesCount;
		uint32_t m_CurrentFrameIndex;
		
		std::vector<uint64_t> m_Fences;
		std::vector<ComPtr<ID3D12CommandAllocator>> m_CommandAllocators;
		std::vector<Ref<FUploadBuffer>> m_UploadBuffers;
	};

	template<typename T>
	FDX12FrameResource<T>::FDX12FrameResource()
	{
		FFrameResourcesManager::Get().RegistFrameResource(this);
	}

	template<typename T>
	inline Ref<T> FDX12FrameResource<T>::GetCurrentBuffer()
	{
		return m_CpuBuffers[FFrameResourcesManager::Get().GetCurrentIndex()];
	}

	template<typename T>
	inline void FDX12FrameResource<T>::UploadCurrentBuffer()
	{
		uint32_t CurrentResIndex = m_CpuBuffers[FFrameResourcesManager::Get().GetCurrentIndex()];
		memcpy(
			m_GPUBuffers[CurrentResIndex]->CPU,
			m_CPUBuffers[CurrentResIndex].get()
			, sizeof(T)
		);
	}

	template<typename T>
	inline D3D12_GPU_VIRTUAL_ADDRESS FDX12FrameResource<T>::GetCurrentGPUAddress()
	{
		return m_GpuBuffers[FFrameResourcesManager::Get().GetCurrentIndex()].GPU;
	}
}