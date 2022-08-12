#include "ResourceStateTracker.h"

namespace Zero
{
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
			
			// First check if there is already a known "final" state for the given resource.
			// If there is, the resource has been used on the command list before and
			// already has a known state within the command list execution.
			const auto Iter = FinalResourceState.find(TransitionBarrier.pResource);
			if (Iter != FinalResourceState.end())
			{
				auto& ResourceState = Iter->second;
				// If the known final state of the resource is different...
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
							ResourceBarriersList.push_back(NewBarrier);
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
						ResourceBarriersList.push_back(NewBarrier);
					}
				}
			}
			else  //In this case, the resource is being used on the command list for the first time.
			{
				// Add a pending barrier. The pending barriers will be resolved
				// before the command list is executed on the command queue.
				PendingResourceBarriersList.push_back(Barrier);
			}

			// Push the final known state (possibly replacing the previously known state for the subresource).
			FinalResourceState[TransitionBarrier.pResource].SetSubResourceState(TransitionBarrier.Subresource, TransitionBarrier.StateAfter);
		}
		else
		{
			// Just push non-transition barriers to the resource barriers array
			ResourceBarriersList.push_back(Barrier);
		}
	}

	void FResourceStateTracker::TransitionResource(ID3D12Resource* Resource, D3D12_RESOURCE_STATES StateAfter, UINT SubResource)
	{
		if (Resource != nullptr)
		{
			ResourceBarrier(CD3DX12_RESOURCE_BARRIER::Transition(Resource, D3D12_RESOURCE_STATE_COMMON, StateAfter, SubResource));
		}
	}
	void FResourceStateTracker::TransitionResource(const FResource& Resource, D3D12_RESOURCE_STATES StateAfter, UINT SubResource)
	{
		TransitionResource(Resource.GetD3DResource().Get(), StateAfter, SubResource);
	}

	uint32_t FResourceStateTracker::FlushPendingResourceBarriers(const Ref<FDX12CommandList>& CommandList)
	{
		CORE_ASSERT(s_bLocked, "Barrier is unlocked!");
		
		FResourceBarriers ToExcuteBarriers;
		ToExcuteBarriers.reserve(PendingResourceBarriersList.size());
		for (auto PendingBarrier : PendingResourceBarriersList)
		{
			if (PendingBarrier.Type == D3D12_RESOURCE_BARRIER_TYPE_TRANSITION)
			{
				D3D12_RESOURCE_TRANSITION_BARRIER PendingTransiton = PendingBarrier.Transition;
				
				auto Iter = s_GlobalResourceState.find(PendingTransiton.pResource);
				if (Iter != s_GlobalResourceState.end())
				{
					// If all subresources are being transitioned, and there are multiple
					// subresources of the resource that are in a different state...
					
				}
			}
		}
	}

	void FResourceStateTracker::FlushResourceBarriers(const Ref<FDX12CommandList>& CommandList)
	{
		UINT NumBarriers = static_cast<UINT> (ResourceBarriersList.size());
		if (NumBarriers > 0)
		{
			CommandList->GetD3D12CommandList()->ResourceBarrier(NumBarriers, ResourceBarriersList.data());
			ResourceBarriersList.clear();
		}
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
