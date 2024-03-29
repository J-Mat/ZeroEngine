#pragma once

#include "Core.h"
#include "../../Common/DX12Header.h"


namespace Zero
{
	class FDX12Device;
	class FUploadBuffer
	{
	public:
		struct FAllocation
		{
			void* CPU;
			D3D12_GPU_VIRTUAL_ADDRESS GPU;
		};

		FUploadBuffer(size_t PageSize = _2MB);
		virtual ~FUploadBuffer() {}
		
		FAllocation Allocate(size_t sizeInBytes, size_t alignment);
		void Reset();
	private:
		struct FPage
		{
			FPage(size_t InSizeInBytes);
			~FPage();

			bool HasSpace(size_t SizeInBytes, size_t Alignment) const;

			FAllocation Allocate(size_t SizeInBytes, size_t Alignment);
			
			void Reset();
		private:
	
			ComPtr<ID3D12Resource> Resource;

			void* m_CPUPtr;
			D3D12_GPU_VIRTUAL_ADDRESS m_GPUPtr;
	
			size_t m_PageSize;
			size_t m_Offset;
		};
		
		Ref<FPage> RequestPage();

		using FPagePool = std::deque<Ref<FPage>>;


		FPagePool m_PagePool;
		FPagePool m_AvaliablePages;
		Ref<FPage> m_CurrentPage;
		size_t m_PageSize;
	};
	
}