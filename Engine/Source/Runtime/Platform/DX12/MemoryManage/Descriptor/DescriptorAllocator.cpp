#include "DescriptorAllocator.h"
#include "DescriptorAllocatorPage.h"

namespace Zero
{
    FDescriptorAllocator::FDescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE Type, uint32_t InNumDescriptorsPerHeap)
        : m_HeapType(Type)
        , m_NumDescriptorsPerHeap(InNumDescriptorsPerHeap)
    {
       
        
    }

    FDescriptorAllocator::~FDescriptorAllocator()
    {
    }

    Ref<FDescriptorAllocatorPage> FDescriptorAllocator::CreateAllocatorPage()
    {
        Ref<FDescriptorAllocatorPage> NewPage = CreateRef<FDescriptorAllocatorPage>(m_HeapType, m_NumDescriptorsPerHeap);
        
        m_HeapPool.emplace_back(NewPage);
        m_AvailableHeaps.insert(m_HeapPool.size() - 1);
        
        return NewPage;
    }

    void FDescriptorAllocator::ReleaseStaleDescriptors()
    {
        std::lock_guard<std::mutex> Lock(m_AllocationMutex);
       
        for (size_t i = 0; i < m_HeapPool.size(); ++i)
        {
            auto Page = m_HeapPool[i];
            Page->ReleaseStaleDescriptors();
            if (Page->GetNumFreeHandles() > 0)
            {
                m_AvailableHeaps.insert(i);
            }
        }
    }

    FDescriptorAllocation FDescriptorAllocator::Allocate(uint32_t NumDescriptors)
    {
        std::lock_guard<std::mutex> Lock(m_AllocationMutex);
        
        FDescriptorAllocation Allocation;

        auto Iter = m_AvailableHeaps.begin();

        while (Iter != m_AvailableHeaps.end())
        {
            size_t Index = *Iter;
            auto AllocatorPage = m_HeapPool[Index];
            
            Allocation = AllocatorPage->Allocate(NumDescriptors);
            
            if (AllocatorPage->GetNumFreeHandles() == 0)
            {
                Iter = m_AvailableHeaps.erase(Iter);
            }
            else
            {
                ++Iter;
            }
            if (!Allocation.IsNull())
            {
                break;
            }
        }
        
        if (Allocation.IsNull())
        {
            m_NumDescriptorsPerHeap = std::max(m_NumDescriptorsPerHeap, NumDescriptors);
            auto NewPage = CreateAllocatorPage();
            Allocation = NewPage->Allocate(NumDescriptors);
        }
        
        return Allocation;
    }
}
