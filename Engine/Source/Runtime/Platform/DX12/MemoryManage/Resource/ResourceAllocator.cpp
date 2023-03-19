#include "ResourceAllocator.h"
#include "../../DX12Device.h"
#include "Core/Framework/Application.h"
#include "Core/Base/FrameTimer.h"

namespace Zero
{
	FBuddyAllocator::FBuddyAllocator(const FAllocatorInitData& InitData)
		: m_InitData(InitData)
	{
		switch (InitData.AllocationStrategy)
		{
		case  EAllocationStrategy::PlacedResource:
		{
			CD3DX12_HEAP_PROPERTIES HeapProperties(InitData.HeapType);
			D3D12_HEAP_DESC Desc = {
				.SizeInBytes = DEFAULT_POOL_SIZE,
				.Properties = HeapProperties,
				.Alignment = 0,
				.Flags = InitData.HeapFlags
			};

			// Create BackingHeap, we will create place resources on it.
			ThrowIfFailed(FDX12Device::Get()->GetDevice()->CreateHeap(&Desc, IID_PPV_ARGS(&m_BackingHeap)));
			m_BackingHeap->SetName(L"BuddyAllocator BackingHeap");
			break;
		}
		case  EAllocationStrategy::ManualSubAllocation:
		{
			CD3DX12_HEAP_PROPERTIES HeapProperties(InitData.HeapType);
			D3D12_RESOURCE_STATES HeapResourceStates;
			if (InitData.HeapType == D3D12_HEAP_TYPE_UPLOAD)
			{
				HeapResourceStates = D3D12_RESOURCE_STATE_GENERIC_READ;
			}
			else if (InitData.HeapType == D3D12_HEAP_TYPE_READBACK)
			{
				HeapResourceStates = D3D12_RESOURCE_STATE_COPY_DEST;
			}
			else //D3D12_HEAP_TYPE_DEFAULT
			{
				HeapResourceStates = D3D12_RESOURCE_STATE_COMMON;
			}
			CD3DX12_RESOURCE_DESC BufferDesc = CD3DX12_RESOURCE_DESC::Buffer(DEFAULT_POOL_SIZE, InitData.ResourceFlags);
			// Create committed resource, we will allocate sub regions on it.
			ComPtr<ID3D12Resource> D3DResource;
			ThrowIfFailed(FDX12Device::Get()->GetDevice()->CreateCommittedResource(
				&HeapProperties,
				D3D12_HEAP_FLAG_NONE,
				&BufferDesc,
				HeapResourceStates,
				nullptr,
				IID_PPV_ARGS(&D3DResource)));
			m_BackingResource = CreateRef<FDX12Resource>("BackingResource", D3DResource);

			if (InitData.HeapType == D3D12_HEAP_TYPE_UPLOAD)
			{
				m_BackingResource->Map();
			}
			break;
		}
		}

		// Initialize free blocks, add the free block for MaxOrder
		m_MaxOrder = UnitSizeToOrder(SizeToUnitSize(DEFAULT_POOL_SIZE));
		for (uint32_t i = 0; i <= m_MaxOrder; ++i)
		{
			m_FreeBlocks.emplace_back(std::set<uint32_t>());
		}

		m_FreeBlocks[m_MaxOrder].insert((uint32_t)0);
	}

	FBuddyAllocator::~FBuddyAllocator()
	{
	}

	bool FBuddyAllocator::CanAllocate(uint32_t SizeToAllocate)
	{
		if (m_TotalAllocSize == DEFAULT_POOL_SIZE)
		{
			return false;
		}
		uint32_t BlockSize = DEFAULT_POOL_SIZE;

		for (size_t i = m_FreeBlocks.size() - 1; i >= 0; --i)
		{ 
			if (m_FreeBlocks[i].size() > 0 && BlockSize >= SizeToAllocate)
			{
				return true;
			}

			BlockSize >>= 1;
			if (BlockSize < SizeToAllocate)
			{
				return false;
			}
		}
		return false;
	}

	uint32_t FBuddyAllocator::AllocateBlock(uint32_t Order)
	{
		uint32_t Offset;
		CORE_ASSERT(Order <= m_MaxOrder, "Order is too large!");
		if (m_FreeBlocks[Order].size() == 0)
		{ 
			uint32_t Left = AllocateBlock(Order + 1);

			uint32_t UnitSize = OrderToUnitSize(Order);
			
			uint32_t Right = Left + UnitSize;

			// Add the right block to the free pool  
			m_FreeBlocks[Order].insert(Right);

			Offset = Left;
		}
		else
		{
			auto Iter = m_FreeBlocks[Order].cbegin();
			Offset = *Iter;

			// Remove the block from the free list
			m_FreeBlocks[Order].erase(*Iter);
		}

		return Offset;
	}

	bool FBuddyAllocator::AllocResource(uint32_t Size, uint32_t Alignment, FResourceLocation& ResourceLocation)
	{
		uint32_t SizeToAllocate = ZMath::AlignArbitrary<uint32_t>(Size, Alignment);
		
		if (CanAllocate(SizeToAllocate))
		{ 
			const uint32_t UnitSize = SizeToUnitSize(SizeToAllocate);
			const uint32_t Order = UnitSizeToOrder(UnitSize);
			const uint32_t Offset = AllocateBlock(Order);
			const uint32_t AllocSize = UnitSize * m_MinBlockSize;
			m_TotalAllocSize += AllocSize;

			//Calculate AlignedOffsetFromResourceBase
			const uint32_t OffsetFromBaseOfResource = GetAllocOffsetInBytes(Offset);
			uint32_t AlignedOffsetFromResourceBase = OffsetFromBaseOfResource;
			if (Alignment != 0 && OffsetFromBaseOfResource % Alignment != 0)
			{
				AlignedOffsetFromResourceBase = ZMath::AlignArbitrary(OffsetFromBaseOfResource, Alignment);

				uint32_t Padding = AlignedOffsetFromResourceBase - OffsetFromBaseOfResource;
				CORE_ASSERT((Padding + Size) <= AllocSize, "(Padding + Size) <= AllocSize");
			}
			CORE_ASSERT((AlignedOffsetFromResourceBase % Alignment) == 0, "(AlignedOffsetFromResourceBase % Alignment) == 0");
			
			ResourceLocation.SetType(FResourceLocation::EResourceLocationType::SubAllocation);
			ResourceLocation.m_BlockData.Order = Order;
			ResourceLocation.m_BlockData.Offset = Offset;
			ResourceLocation.m_BlockData.ActualUsedSize = Size;
			ResourceLocation.m_Allocator = this;

			if (m_InitData.AllocationStrategy == EAllocationStrategy::ManualSubAllocation)
			{
				ResourceLocation.m_UnderlyingResource = m_BackingResource;
				ResourceLocation.m_OffsetFromBaseOfResource = AlignedOffsetFromResourceBase;
				ResourceLocation.m_GPUVirtualAddress = m_BackingResource->m_GPUVirtualAddress + AlignedOffsetFromResourceBase;

				if (m_InitData.HeapType == D3D12_HEAP_TYPE_UPLOAD)
				{
					ResourceLocation.m_MappedAddress = ((uint8_t*)m_BackingResource->m_MappedBaseAddress + AlignedOffsetFromResourceBase);
				}
			}
			else
			{
				ResourceLocation.m_OffsetFromBaseOfHeap = AlignedOffsetFromResourceBase;
			}

			return true;
		}
		return false;
	}

	void FBuddyAllocator::DeallocateBlock(uint32_t Offset, uint32_t Order)
	{
		uint32_t Size = OrderToUnitSize(Order);
		uint32_t TheOtherBuddy = (Offset ^ Size);
		auto Iter = m_FreeBlocks[Order].find(TheOtherBuddy);
		if (Iter != m_FreeBlocks[Order].end()) // // If buddy block is free, merge it
		{
			// Deallocate merged blocks
			DeallocateBlock(std::min(Offset, TheOtherBuddy), Order + 1);

			// Remove the buddy from the free list  
			m_FreeBlocks[Order].erase(*Iter);
		}
		else
		{
			// Add the block to the free list
			m_FreeBlocks[Order].insert(Offset);
		}
	}

	void FBuddyAllocator::DeallocateInternal(FBuddyBlockData& Block)
	{
		DeallocateBlock(Block.Offset, Block.Order);
		uint32_t Size = OrderToUnitSize(Block.Order) * m_MinBlockSize;
		m_TotalAllocSize -= Size;

		if (m_InitData.AllocationStrategy == EAllocationStrategy::PlacedResource)
		{
			Block.PlacedResource.reset();
		}
	}

	void FBuddyAllocator::Deallocate(FResourceLocation& ResourceLocation)
	{
		uint64_t FrameCount = FApplication::Get().GetFrameTimer()->GetFrameCount();
		m_DeferredDeletionQueue.push_back({ ResourceLocation.m_BlockData, FrameCount });
	}

	void FBuddyAllocator::CleanUpAllocations()
	{
		while (!m_DeferredDeletionQueue.empty())
		{ 
			auto Iter = m_DeferredDeletionQueue.front();
			uint64_t CurrentFrameCount = FApplication::Get().GetFrameTimer()->GetFrameCount();
			if (Iter.second < CurrentFrameCount)
			{
				FBuddyBlockData& Block = Iter.first;
				DeallocateInternal(Block);
				m_DeferredDeletionQueue.pop_front();
			}
			else
			{
				break;
			}
		}
	}

	uint32_t FBuddyAllocator::OrderToUnitSize(uint32_t Order) const
	{
		return ((uint32_t)1) << Order;
	}

	inline uint32_t FBuddyAllocator::SizeToUnitSize(uint32_t Size) const
	{
		return (Size + (m_MinBlockSize - 1)) / m_MinBlockSize;
	}

	inline uint32_t FBuddyAllocator::UnitSizeToOrder(uint32_t Size) const
	{
		unsigned long Result;
		//uint32_t tt = ZMath::CalLog2Interger(Size);
		_BitScanReverse(&Result, Size + Size - 1); // ceil(log2(size))
		//CORE_ASSERT(tt == Result, "invalid!");
		return Result;
	}

	inline uint32_t FBuddyAllocator::GetAllocOffsetInBytes(uint32_t Offset) const
	{
		return Offset * m_MinBlockSize;
	}

	FMultiBuddyAllocator::FMultiBuddyAllocator(const FBuddyAllocator::FAllocatorInitData& InitData)
		: m_InitData(InitData)
	{
	}
	FMultiBuddyAllocator::~FMultiBuddyAllocator()
	{
	}
	bool FMultiBuddyAllocator::AllocResource(uint32_t Size, uint32_t Alignment, FResourceLocation& ResourceLocation)
	{
		for (auto& Allocator : m_Allocators)
		{ 
			if (Allocator->AllocResource(Size, Alignment, ResourceLocation))
			{
				return true;
			}
		}
		auto Allocator = CreateRef<FBuddyAllocator>(m_InitData);
		m_Allocators.push_back(Allocator);
		Allocator->AllocResource(Size, Alignment, ResourceLocation);
		return true;
	}

	void FMultiBuddyAllocator::CleanUpAllocations()
	{
		for (auto& Allocator : m_Allocators)
		{
			Allocator->CleanUpAllocations();
		}
	}

	FTextureResourceAllocator::FTextureResourceAllocator()
	{
		FBuddyAllocator::FAllocatorInitData InitData = 
		{
			.AllocationStrategy = FBuddyAllocator::EAllocationStrategy::PlacedResource,
			.HeapType = D3D12_HEAP_TYPE_DEFAULT,
			.HeapFlags = D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES
		};
		m_Allocator = CreateScope<FMultiBuddyAllocator>(InitData);
	}

	void FTextureResourceAllocator::AllocTextureResource(const std::string& TextureName, const D3D12_RESOURCE_STATES& ResourceState, const D3D12_RESOURCE_DESC& ResourceDesc, FResourceLocation& ResourceLocation)
	{
		auto* D3DDeviec = FDX12Device::Get()->GetDevice();
		const D3D12_RESOURCE_ALLOCATION_INFO Info = D3DDeviec->GetResourceAllocationInfo(0, 1, &ResourceDesc);
		m_Allocator->AllocResource((uint32_t)Info.SizeInBytes, DEFAULT_RESOURCE_ALIGNMENT, ResourceLocation);

		ComPtr<ID3D12Resource> D3DResource;
		ID3D12Heap* BackingHeap = ResourceLocation.m_Allocator->GetBackingHeap();
		uint64_t HeapOffset = ResourceLocation.m_OffsetFromBaseOfHeap;
		D3DDeviec->CreatePlacedResource(BackingHeap, HeapOffset, &ResourceDesc, ResourceState, nullptr, IID_PPV_ARGS(&D3DResource));

		ResourceLocation.m_UnderlyingResource = CreateRef<FDX12Resource>(TextureName, D3DResource);
		ResourceLocation.m_BlockData.PlacedResource = ResourceLocation.m_UnderlyingResource;
	}

	void FTextureResourceAllocator::CleanUpAllocations()
	{
		m_Allocator->CleanUpAllocations();
	}
	FUploadBufferAllocator::FUploadBufferAllocator()
	{
		FBuddyAllocator::FAllocatorInitData InitData =
		{
			.AllocationStrategy = FBuddyAllocator::EAllocationStrategy::ManualSubAllocation,
			.HeapType = D3D12_HEAP_TYPE_UPLOAD,
			.ResourceFlags = D3D12_RESOURCE_FLAG_NONE
		};
		m_Allocator = CreateScope<FMultiBuddyAllocator>(InitData);
	}

	void* FUploadBufferAllocator::AllocUploadResource(uint64_t Size, uint32_t Alignment, FResourceLocation& ResourceLocation)
	{
		m_Allocator->AllocResource((uint32_t)Size, Alignment, ResourceLocation);
		return ResourceLocation.m_MappedAddress;
	}

	void FUploadBufferAllocator::CleanUpAllocations()
	{
		m_Allocator->CleanUpAllocations();
	}

	FDefaultBufferAllocator::FDefaultBufferAllocator()
	{
		{
			FBuddyAllocator::FAllocatorInitData InitData{
				.AllocationStrategy = FBuddyAllocator::EAllocationStrategy::ManualSubAllocation,
				.HeapType = D3D12_HEAP_TYPE_DEFAULT,
				.ResourceFlags = D3D12_RESOURCE_FLAG_NONE
			};

			m_Allocator = CreateScope<FMultiBuddyAllocator>(InitData);
		}

		{
			FBuddyAllocator::FAllocatorInitData InitData{
				.AllocationStrategy = FBuddyAllocator::EAllocationStrategy::ManualSubAllocation,
				.HeapType = D3D12_HEAP_TYPE_DEFAULT,
				.ResourceFlags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS
			};

			m_UavAllocator = CreateScope<FMultiBuddyAllocator>(InitData);
		}
	}

	void FDefaultBufferAllocator::AllocDefaultResource(const D3D12_RESOURCE_DESC& ResourceDesc, uint32_t Alignment, FResourceLocation& ResourceLocation)
	{
		if (ResourceDesc.Flags == D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS)
		{
			m_UavAllocator->AllocResource((uint32_t)ResourceDesc.Width, Alignment, ResourceLocation);
		}
		else
		{
			m_Allocator->AllocResource((uint32_t)ResourceDesc.Width, Alignment, ResourceLocation);
		}
	}

	void FDefaultBufferAllocator::CleanUpAllocations()
	{
		m_Allocator->CleanUpAllocations();
		m_UavAllocator->CleanUpAllocations();
	}
}