#pragma once
#include "Core.h"
#include "Platform/DX12/Common/DX12Header.h"


namespace Zero
{
	class FDX12Device;
	class FDescriptorAllocation;
	class FDescriptorAllocatorPage : public std::enable_shared_from_this<FDescriptorAllocatorPage>
	{
	public:
		D3D12_DESCRIPTOR_HEAP_TYPE GetHeapType() const { return m_HeapType; }
		FDescriptorAllocatorPage(D3D12_DESCRIPTOR_HEAP_TYPE Type, uint32_t NumDescriptors);

		bool HasSpace(uint32_t NumDescriptors);
		uint32_t GetNumFreeHandles() const;
		void AddNewBlock(uint32_t Offset, uint32_t NumDescriptors);
		void FreeBlock(uint32_t Offset, uint32_t NumDescriptors);
		void Free(FDescriptorAllocation&& m_Descriptor);
		uint32_t ComputeOffset(D3D12_CPU_DESCRIPTOR_HANDLE Handle);

		FDescriptorAllocation Allocate(uint32_t NumDescriptors);
		void ReleaseStaleDescriptors();
		ComPtr<ID3D12DescriptorHeap> GetDescriptorHeap() { return m_D3DDescriptorHeap; }
	private:
		struct FFreeBlockInfo;

		using FFreeListByOffset = std::map<uint32_t, FFreeBlockInfo>;
		using FFreeListBySize = std::multimap<uint32_t, FFreeListByOffset::iterator>;

		struct FFreeBlockInfo
		{
			FFreeBlockInfo(uint32_t InSize)
				: Size(InSize)
			{}

			uint32_t Size;
			FFreeListBySize::iterator FreeListBySizeIter;
		};

		struct FStaleDescriptorInfo
		{
			FStaleDescriptorInfo(uint32_t InOffset, uint32_t InSize)
				: Offset(InOffset)
				, Size(InSize)
			{}
			// The InOffset within the descriptor heap.
			uint32_t Offset;
			// The number of descriptors
			uint32_t Size;
		};

		using FStaleDescriptorQueue = std::queue<FStaleDescriptorInfo>;

		FFreeListByOffset m_FreeListByOffset;
		FFreeListBySize  m_FreeListBySize;
		FStaleDescriptorQueue m_StaleDescriptorQueue;
		ComPtr<ID3D12DescriptorHeap> m_D3DDescriptorHeap;
		D3D12_DESCRIPTOR_HEAP_TYPE m_HeapType;
		CD3DX12_CPU_DESCRIPTOR_HANDLE m_BaseDescriptor;
		uint32_t  m_DescriptorHandleIncrementSize;
		uint32_t  m_NumDescriptorsInHeap;
		uint32_t  m_NumFreeHandles;
		std::mutex m_AllocationMutex;
	};
}