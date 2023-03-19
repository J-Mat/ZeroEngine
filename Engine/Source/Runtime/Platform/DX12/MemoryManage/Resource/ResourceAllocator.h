#pragma once

#include "Core.h"
#include "../../Common/DX12Header.h"
#include "Resource.h"

namespace Zero
{ 
	#define DEFAULT_POOL_SIZE (512 * 1024 * 512)

	#define DEFAULT_RESOURCE_ALIGNMENT 4
	#define UPLOAD_RESOURCE_ALIGNMENT 256

	class FBuddyAllocator
	{
	public:
		enum class EAllocationStrategy
		{
			PlacedResource = 0,
			ManualSubAllocation
		};

		struct FAllocatorInitData
		{
			EAllocationStrategy AllocationStrategy;

			D3D12_HEAP_TYPE HeapType;

			D3D12_HEAP_FLAGS HeapFlags = D3D12_HEAP_FLAG_NONE;  // Only for PlacedResource

			D3D12_RESOURCE_FLAGS ResourceFlags = D3D12_RESOURCE_FLAG_NONE;  // Only for ManualSubAllocation
		};
	public:
		FBuddyAllocator(const FAllocatorInitData& InInitData);
		~FBuddyAllocator();


		bool CanAllocate(uint32_t SizeToAllocate);
		uint32_t AllocateBlock(uint32_t Order);
		bool AllocResource(uint32_t Size, uint32_t Alignment, FResourceLocation& ResourceLocation);

		void DeallocateBlock(uint32_t Offset, uint32_t Order);
		void DeallocateInternal(FBuddyBlockData& Block);
		void Deallocate(FResourceLocation& ResourceLocation);

		void CleanUpAllocations();

		ID3D12Heap* GetBackingHeap() { return m_BackingHeap; }

	private:
		uint32_t OrderToUnitSize(uint32_t Order) const;

		inline uint32_t SizeToUnitSize(uint32_t Size) const;

		inline uint32_t UnitSizeToOrder(uint32_t Size) const;

		inline uint32_t GetAllocOffsetInBytes(uint32_t Offset) const;

	private:
		FAllocatorInitData m_InitData;

		const uint32_t m_MinBlockSize = 256;

		uint32_t m_MaxOrder;

		uint32_t m_TotalAllocSize = 0;

		std::vector<std::set<uint32_t>> m_FreeBlocks;

		using DeleteBlockDataInFrame = std::pair<FBuddyBlockData, uint64_t>;
		std::deque<DeleteBlockDataInFrame> m_DeferredDeletionQueue;

		Ref<FDX12Resource> m_BackingResource = nullptr;

		ID3D12Heap* m_BackingHeap = nullptr;
	};


	class FMultiBuddyAllocator
	{
	public:
		FMultiBuddyAllocator(const FBuddyAllocator::FAllocatorInitData& InitData);
		~FMultiBuddyAllocator();

		bool AllocResource(uint32_t Size, uint32_t Alignment, FResourceLocation& ResourceLocation);

		void CleanUpAllocations();
	private:
		std::vector<Ref<FBuddyAllocator>> m_Allocators;
		FBuddyAllocator::FAllocatorInitData m_InitData;
	};

	class FTextureResourceAllocator
	{
	public:
		FTextureResourceAllocator();
		void AllocTextureResource(const std::string& TextureName,  const D3D12_RESOURCE_STATES& ResourceState, const D3D12_RESOURCE_DESC& ResourceDesc, FResourceLocation& ResourceLocation);
		void CleanUpAllocations();
	private:
		Scope<FMultiBuddyAllocator> m_Allocator = nullptr;
	};

	class FUploadBufferAllocator
	{
	public:
		FUploadBufferAllocator();
		void* AllocUploadResource(uint64_t Size, uint32_t Alignment, FResourceLocation& ResourceLocation);
		void CleanUpAllocations();

	private:
		Scope<FMultiBuddyAllocator> m_Allocator = nullptr;
	};

	class FDefaultBufferAllocator
	{
	public:
		FDefaultBufferAllocator();
		void AllocDefaultResource(const D3D12_RESOURCE_DESC& ResourceDes, uint32_t Alignment,FResourceLocation& ResourceLocation);
		void CleanUpAllocations();

	private:
		Scope<FMultiBuddyAllocator> m_Allocator = nullptr;
		Scope<FMultiBuddyAllocator> m_UavAllocator = nullptr;
	};
}