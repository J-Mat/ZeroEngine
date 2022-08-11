#include "DescriptorAllocator.h"

namespace Zero
{
    FDescriptorAllocator::FDescriptorAllocator(FDX12Device& InDevice, D3D12_DESCRIPTOR_HEAP_TYPE Type, uint32_t InNumDescriptorsPerHeap)
        : Device(InDevice)
        , HeapType(Type)
        , NumDescriptorsPerHeap(InNumDescriptorsPerHeap)
    {
       
        
    }

    FDescriptorAllocator::~FDescriptorAllocator()
    {
    }

    Ref<FDescriptorAllocatorPage> FDescriptorAllocator::CreateAllocatorPage()
    {
        Ref<FDescriptorAllocatorPage> NewPage = CreateRef<FDescriptorAllocatorPage>(Device, HeapType, NumDescriptorsPerHeap);
        
        HeapPool.emplace_back(NewPage);
        AvailableHeaps.insert(HeapPool.size() - 1);
        
        return NewPage;
    }

    void FDescriptorAllocator::ReleaseStaleDescriptors()
    {
        std::lock_guard<std::mutex> Lock(AllocationMutex);
       
        for (size_t i = 0; i < HeapPool.size(); ++i)
        {
            auto Page = HeapPool[i];
            Page->ReleaseStaleDescriptors();
            if (Page->GetNumFreeHandles() > 0)
            {
                AvailableHeaps.insert(i);
            }
        }
    }

    FDescriptorAllocation FDescriptorAllocator::Allocate(uint32_t NumDescriptors)
    {
        std::lock_guard<std::mutex> Lock(AllocationMutex);
        
        FDescriptorAllocation Allocation;

        auto Iter = AvailableHeaps.begin();

        while (Iter != AvailableHeaps.end())
        {
            size_t Index = *Iter;
            auto AllocatorPage = HeapPool[Index];
            
            auto Allocation = AllocatorPage->Allocate(NumDescriptors);
            
            if (AllocatorPage->GetNumFreeHandles() == 0)
            {
                Iter = AvailableHeaps.erase(Iter);
            }
            else
            {
                ++Iter;
            }
            if (Allocation.IsNull())
            {
                break;
            }
        }
        
        if (Allocation.IsNull())
        {
            NumDescriptorsPerHeap = std::max(NumDescriptorsPerHeap, NumDescriptors);
            auto NewPage = CreateAllocatorPage();
            Allocation = NewPage->Allocate(NumDescriptors);
        }
        
        return Allocation;
    }
}
