#pragma once

#include "Core.h"
#include "../Common/DX12Header.h"


namespace Zero
{
	class FUploadBuffer
	{
	public:
		struct FAllocation
		{
			void* CPU;
			D3D12_GPU_VIRTUAL_ADDRESS GPU;
		};

		FAllocation Allocate(size_t sizeInBytes, size_t alignment);
		void Reset();
	private:
		struct FPage
		{
			FPage(FDX12Device& InDevice, size_t InSizeInBytes);
			~FPage();

			bool HasSpace(size_t SizeInBytes, size_t Alignment) const;

			FAllocation Allocate(size_t SizeInBytes, size_t Alignment);
			
			void Reset();
		private:
			FDX12Device& Device;
			ComPtr<ID3D12Resource> Resource;

			void* CPUPtr;
			D3D12_GPU_VIRTUAL_ADDRESS GPUPtr;
	
			size_t PageSize;
			size_t Offset;
		};
		
		Ref<FPage> RequestPage();

		using FPagePool = std::deque<Ref<FPage>>;

		FDX12Device& Device;
		FPagePool PagePool;
		FPagePool AvaliablePages;
		Ref<FPage> CurrentPage;
		size_t PageSize;
	};
	
}