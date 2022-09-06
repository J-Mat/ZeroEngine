#pragma once

#include "Core.h"
#include "../Common/DX12Header.h"
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
		FFrameResourceBuffer(uint32_t Size);
		~FFrameResourceBuffer()
		{
			delete m_CPUBuffer;
		}
		void CopyDataToConstantsBuffer(void* Data, uint32_t Offset, uint32_t Length)
		{
			void* Target = (void*)((char*)m_CPUBuffer + Offset);
			memcpy(Target, Data, Length);
		}

		void CopyDataFromConstantsBuffer(void* Data, uint32_t Offset, uint32_t Length)
		{
			void* ConstBuffer = (void*)((char*)m_CPUBuffer + Offset);
			memcpy(Data, ConstBuffer, Length);

		}
		void* GetPtrFromConstantsBuffer(uint32_t Offset)
		{
			return (void*)((char*)m_CPUBuffer + Offset);
		}
		D3D12_GPU_VIRTUAL_ADDRESS GetCurrentGPUAddress();

		void* GetBuffer() { return m_CPUBuffer; }

		void UploadCurrentBuffer();
		uint32_t GetSizeInByte() { return m_SizeByte; }
	private:
		std::vector<FUploadBuffer::FAllocation> m_GPUBuffers;
		void* m_CPUBuffer;
		uint32_t m_SizeByte;;
	};


	class FFrameResourcesManager : public IPublicSingleton<FFrameResourcesManager>
	{
	public:
		FFrameResourcesManager(int FrameResourcesCount = 3);
		virtual ~FFrameResourcesManager();
		void Init(Ref<FDX12Device> Device);
		void SetCurrentFence(uint64_t FenceValue);
		void UseNextFrameResource() { m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % m_FrameResourcesCount; }
		uint32_t GetCurrentIndex() { return m_CurrentFrameIndex; }


		template<typename T>
		void RegistFrameResource(FDX12FrameResource<T>* FrameResource)
		{
			FrameResource->m_CPUBuffers.reserve(m_FrameResourcesCount);
			FrameResource->m_GPUBuffers.reserve(m_FrameResourcesCount);
			for (int i = 0; i < m_FrameResourcesCount; ++i)
			{
				FrameResource->m_CPUBuffers[i].reset(new T);
				FrameResource->m_GPUBuffers[i] = m_UploadBuffers[i]->Allocate(sizeof(T), true);
			}
			new FUploadBuffer::FAllocation
		}

		void RegistFrameResource(FFrameResourceBuffer* FrameResourceBuffer)
		{
			FrameResourceBuffer->m_CPUBuffer = new byte[FrameResourceBuffer->m_SizeByte];
			FrameResourceBuffer->m_GPUBuffers.reserve(m_FrameResourcesCount);
			for (uint32_t i = 0; i < m_FrameResourcesCount; ++i)
			{

				FrameResourceBuffer->m_GPUBuffers[i] = m_UploadBuffers[i]->Allocate(FrameResourceBuffer->m_SizeByte, true);
			}
		}

	private:
		uint32_t m_FrameResourcesCount;
		uint32_t m_CurrentFrameIndex;
		Ref<FDX12Device> m_Device;
		std::vector<uint64_t> m_Fences;
		std::vector<ComPtr<ID3D12CommandAllocator>> m_CommandAllocators;
		std::vector<Ref<FUploadBuffer>> m_UploadBuffers;
	};

	template<typename T>
	FDX12FrameResource<T>::FDX12FrameResource()
	{
		FFrameResourcesManager::GetInstance().RegistFrameResource(this);
	}

	template<typename T>
	inline Ref<T> FDX12FrameResource<T>::GetCurrentBuffer()
	{
		return m_CpuBuffers[FFrameResourcesManager::GetInstance().GetCurrentIndex()];
	}

	template<typename T>
	inline void FDX12FrameResource<T>::UploadCurrentBuffer()
	{
		uint32_t CurrentResIndex = m_CpuBuffers[FFrameResourcesManager::GetInstance().GetCurrentIndex()];
		memcpy(
			m_GPUBuffers[CurrentResIndex]->CPU,
			m_CPUBuffers[CurrentResIndex].get()
			, sizeof(T)
		);
	}

	template<typename T>
	inline D3D12_GPU_VIRTUAL_ADDRESS FDX12FrameResource<T>::GetCurrentGPUAddress()
	{
		return m_GpuBuffers[FFrameResourcesManager::GetInstance().GetCurrentIndex()].GPU;
	}
}