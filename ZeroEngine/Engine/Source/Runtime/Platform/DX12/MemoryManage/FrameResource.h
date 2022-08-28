#pragma once

#include "Core.h"
#include "../Common/DX12Header.h"
#include "Core/Base/PublicSingleton.h"
#include "UploadBuffer.h"



namespace Zero
{
	class FDX12Device;
	class FFrameResourcesManager;
	class FUploadBuffer;

	template<typename T>
	class FDX12FrameResource
	{
	public:
		friend class FFrameResourcesManager;
		FDX12FrameResource()
		{
			FFrameResourcesManager::GetInstance().RegistFrameResource(this);
		}
		Ref<T> GetCurrentBuffer() { return m_CpuBuffers[FFrameResourcesManager::GetInstance().GetCurrentIndex()]; }
		void UploadCurrentBuffer()
		{
			uint32_t CurrentResIndex = m_CpuBuffers[FFrameResourcesManager::GetInstance().GetCurrentIndex()];
			memcpy(
				m_GPUBuffers[CurrentResIndex]->CPU,
				m_CPUBuffers[CurrentResIndex].get()
				, sizeof(T)
			);
		}
		D3D12_GPU_VIRTUAL_ADDRESS GetCurrentGPUAddress()
		{
			return m_GpuBuffers[FFrameResourcesManager::GetInstance().GetCurrentIndex()].GPU;
		}
	private:
		std::vector<FUploadBuffer::FAllocation> m_GPUBuffers;
		std::vector<Ref<T>> m_CPUBuffers;
	};
	
	class FFrameResourcesManager : public IPublicSingleton<FFrameResourcesManager>
	{
	public:
		FFrameResourcesManager(int FrameResourcesCount = 3);
		void Init(Ref<FDX12Device> Device);
		virtual ~FFrameResourcesManager();
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
	private:
		uint32_t m_FrameResourcesCount;
		uint32_t m_CurrentFrameIndex;
		Ref<FDX12Device> m_Device;
		std::vector<uint64_t> m_Fences;
		std::vector<ComPtr<ID3D12CommandAllocator>> m_CommandAllocators;
		std::vector<Ref<FUploadBuffer>> m_UploadBuffers;
	};
}