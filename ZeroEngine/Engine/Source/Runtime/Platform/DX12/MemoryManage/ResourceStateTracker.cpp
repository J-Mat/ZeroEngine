#include "ResourceStateTracker.h"
#include "../DX12CommandList.h"
#include "../Base/Resource.h"

namespace Zero
{
	FResourceStateTracker::ResourceStateMap FResourceStateTracker::s_GlobalResourceState;
	bool  FResourceStateTracker::s_bLocked = false;
	std::mutex FResourceStateTracker::s_GlobalMutex;

	static std::mutex s_GlobalMutex;
	static bool       s_bLocked;
	FResourceStateTracker::FResourceStateTracker()
	{
	}

	FResourceStateTracker::~FResourceStateTracker()
	{
	}

	void FResourceStateTracker::ResourceBarrier(const D3D12_RESOURCE_BARRIER& Barrier)
	{
		if (Barrier.Type == D3D12_RESOURCE_BARRIER_TYPE_TRANSITION)
		{
			const D3D12_RESOURCE_TRANSITION_BARRIER& TransitionBarrier = Barrier.Transition;
			
			// First check if there is already a known "final" state for the given Resource.
			// If there is, the Resource has been used on the command list before and
			// already has a known state within the command list execution.
			const auto Iter = m_FinalResourceState.find(TransitionBarrier.pResource);
			if (Iter != m_FinalResourceState.end())
			{
				auto& ResourceState = Iter->second;
				// If the known final state of the Resource is different...
				if (TransitionBarrier.Subresource == D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES &&
					!ResourceState.SubResourceState.empty())
				{
					for (auto SubResourceState : ResourceState.SubResourceState)
					{
						if (TransitionBarrier.StateAfter != SubResourceState.second)
						{
							D3D12_RESOURCE_BARRIER NewBarrier = Barrier;
							NewBarrier.Transition.Subresource = SubResourceState.first;
							NewBarrier.Transition.StateBefore = SubResourceState.second;
							m_ResourceBarriersList.push_back(NewBarrier);
						}
					}
				}
				else
				{
					auto FinalState = ResourceState.GetSubresourceState(TransitionBarrier.Subresource);
					if (TransitionBarrier.StateAfter != FinalState)
					{
						D3D12_RESOURCE_BARRIER NewBarrier = Barrier;
						NewBarrier.Transition.StateBefore = FinalState;
						m_ResourceBarriersList.push_back(NewBarrier);
					}
				}
			}
			else  //In this case, the Resource is being used on the command list for the first time.
			{
				// Add a pending barrier. The pending barriers will be resolved
				// before the command list is executed on the command queue.
				m_PendingResourceBarriersList.push_back(Barrier);
			}

			// Push the final known state (possibly replacing the previously known state for the subresource).
			m_FinalResourceState[TransitionBarrier.pResource].SetSubResourceState(TransitionBarrier.Subresource, TransitionBarrier.StateAfter);
		}
		else
		{
			// Just push non-transition barriers to the Resource barriers array
			m_ResourceBarriersList.push_back(Barrier);
		}
	}

	void FResourceStateTracker::TransitionResource(ID3D12Resource* Resource, D3D12_RESOURCE_STATES StateAfter, UINT SubResource)
	{
		if (Resource != nullptr)
		{
			ResourceBarrier(CD3DX12_RESOURCE_BARRIER::Transition(Resource, D3D12_RESOURCE_STATE_COMMON, StateAfter, SubResource));
		}
	}

	void FResourceStateTracker::TransitionResource(FResource& Resource, D3D12_RESOURCE_STATES StateAfter, UINT SubResource)
	{
		TransitionResource(Resource.GetD3DResource().Get(), StateAfter, SubResource);
	}

	void FResourceStateTracker::UAVBarrier(const FResource* Resource)
	{
		ID3D12Resource* pResource = Resource != nullptr ? Resource->GetD3DResource().Get() : nullptr;

		ResourceBarrier(CD3DX12_RESOURCE_BARRIER::UAV(pResource));
	}

	void FResourceStateTracker::AliasBarrier(const FResource* ResourceBefore, const FResource* ResourceAfter)
	{
		ID3D12Resource* pResourceBefore = ResourceBefore != nullptr ? ResourceBefore->GetD3DResource().Get() : nullptr;
		ID3D12Resource* pResourceAfter = ResourceAfter != nullptr ? ResourceAfter->GetD3DResource().Get() : nullptr;

		ResourceBarrier(CD3DX12_RESOURCE_BARRIER::Aliasing(pResourceBefore, pResourceAfter));
	}

	uint32_t FResourceStateTracker::FlushPendingResourceBarriers(const Ref<FDX12CommandList>& CommandList)
	{
		//CORE_ASSERT(s_bLocked, "Barrier is unlocked!");
		
		FResourceBarriers ToExcuteBarriers;
		ToExcuteBarriers.reserve(m_PendingResourceBarriersList.size());
		for (auto PendingBarrier : m_PendingResourceBarriersList)
		{
			if (PendingBarrier.Type == D3D12_RESOURCE_BARRIER_TYPE_TRANSITION)
			{
				D3D12_RESOURCE_TRANSITION_BARRIER PendingTransiton = PendingBarrier.Transition;
				
				auto Iter = s_GlobalResourceState.find(PendingTransiton.pResource);
				if (Iter != s_GlobalResourceState.end())
				{
					// If all subresources are being transitioned, and there are multiple
					// subresources of the Resource that are in a different state...
					FResourcestate& Resourcestate = Iter->second;
					if (PendingTransiton.Subresource == D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES &&
						!Resourcestate.SubResourceState.empty())
					{
						// Transition all subresources
						for (auto SubsourceStateIter : Resourcestate.SubResourceState)
						{
							if (PendingTransiton.StateAfter != SubsourceStateIter.second)
							{
								D3D12_RESOURCE_BARRIER NewBarrier = PendingBarrier;
								NewBarrier.Transition.Subresource = SubsourceStateIter.first;
								NewBarrier.Transition.StateBefore = SubsourceStateIter.second;
								ToExcuteBarriers.push_back(NewBarrier);
							}
						}
					}
					else
					{
						// No (sub)resources need to be transitioned. Just add a single transition barrier (if needed).
						auto GlobalState = Iter->second.GetSubresourceState(PendingTransiton.Subresource);
						if (PendingTransiton.StateAfter != GlobalState)
						{
							// Fix-up the before state based on current global state of the Resource.
							PendingBarrier.Transition.StateBefore = GlobalState;
							ToExcuteBarriers.push_back(PendingBarrier);
						}
					}
				}
			}
		}
		
		UINT NumBarriers = static_cast<UINT>(ToExcuteBarriers.size());
		if (NumBarriers > 0)
		{
			CommandList->GetD3D12CommandList()->ResourceBarrier(NumBarriers, ToExcuteBarriers.data());
		}

		m_PendingResourceBarriersList.clear();

		return NumBarriers;
	}

	void FResourceStateTracker::FlushResourceBarriers(const Ref<FDX12CommandList>& CommandList)
	{
		UINT NumBarriers = static_cast<UINT> (m_ResourceBarriersList.size());
		if (NumBarriers > 0)
		{
			CommandList->GetD3D12CommandList()->ResourceBarrier(NumBarriers, m_ResourceBarriersList.data());
			m_ResourceBarriersList.clear();
		}
	}

	void FResourceStateTracker::CommitFinalResourceStates()
	{
		//CORE_ASSERT(s_bLocked, "s_bLocked is unlocked");

		// Commit final Resource states to the global Resource state array (map).
		for (const auto& ResourceState : m_FinalResourceState)
		{
			s_GlobalResourceState[ResourceState.first] = ResourceState.second;
		}

		m_FinalResourceState.clear();
	}

	void FResourceStateTracker::Reset()
	{
		m_PendingResourceBarriersList.clear();
		m_ResourceBarriersList.clear();
		m_FinalResourceState.clear();
	}

	void FResourceStateTracker::Lock()
	{
		s_GlobalMutex.lock();
		s_bLocked = true;
	}

	void FResourceStateTracker::Unlock()
	{
		s_GlobalMutex.unlock();
		s_bLocked = false;
	}

	void FResourceStateTracker::AddGlobalResourceState(ID3D12Resource* Resource, D3D12_RESOURCE_STATES State)
	{
		if (Resource != nullptr)
		{
			std::lock_guard<std::mutex> lock(s_GlobalMutex);
			s_GlobalResourceState[Resource].SetSubResourceState(D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, State);
		}
	}
}
