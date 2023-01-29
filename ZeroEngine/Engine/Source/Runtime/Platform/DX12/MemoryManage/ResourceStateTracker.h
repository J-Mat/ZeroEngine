#pragma once
/**
 *  @file ResourceStateTracker.h
 *  @date October 24, 2018
 *  @author Jeremiah van Oosten
 *
 *  @brief Tracks the known state of a (sub)resource within a command list.
 *
 *  The ResourceStateTracker tracks the known state of a (sub)resource within a command list.
 *  It is often difficult (or impossible) to know the current state of a (sub)resource if
 *  it is being used in multiple command lists. For example when doing shadow mapping,
 *  a depth buffer resource is being used as a depth-stencil view in a command list
 *  that is generating the shadow map for a light source, but needs to be used as
 *  a shader-resource view in a command list that is performing shadow mapping. If
 *  the command lists are being generated in separate threads, the exact state of the
 *  resource can't be guaranteed at the moment it is used in the command list.
 *  The ResourceStateTracker class is intended to be used within a command list
 *  to track the state of the resource as it is known within that command list.
 *
 *  @see https://youtu.be/nmB2XMasz2o
 *  @see https://msdn.microsoft.com/en-us/library/dn899226(v=vs.85).aspx#implicit_state_transitions
 */
#include "Core.h"
#include "../Common/DX12Header.h"

namespace Zero
{
    class FDX12CommandList;
    class FResource;
    class FResourceStateTracker
	{
	public:
		FResourceStateTracker();
		virtual ~FResourceStateTracker();

        /**
        * Push a resource barrier to the resource state tracker.
        *
        * @param barrier The resource barrier to push to the resource state tracker.
        */
        void ResourceBarrier(const D3D12_RESOURCE_BARRIER& barrier);

        /**
         * Push a transition resource barrier to the resource state tracker.
         *
         * @param resource The resource to transition.
         * @param stateAfter The state to transition the resource to.
         * @param subResource The subresource to transition. By default, this is D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES
         * which indicates that all subresources should be transitioned to the same state.
         */
        void TransitionResource(ID3D12Resource* resource, D3D12_RESOURCE_STATES stateAfter, UINT subResource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);
        void TransitionResource(FResource& resource, D3D12_RESOURCE_STATES stateAfter, UINT subResource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);

        /**
         * Push a UAV resource barrier for the given resource.
         *
         * @param resource The resource to add a UAV barrier for. Can be NULL which
         * indicates that any UAV access could require the barrier.
         */
        void UAVBarrier(const FResource* resource = nullptr);

        /**
         * Push an aliasing barrier for the given resource.
         *
         * @param beforeResource The resource currently occupying the space in the heap.
         * @param afterResource The resource that will be occupying the space in the heap.
         *
         * Either the beforeResource or the afterResource parameters can be NULL which
         * indicates that any placed or reserved resource could cause aliasing.
         */
        void AliasBarrier(const FResource* ResourceBefore = nullptr, const FResource* ResourceAfter = nullptr);

        /**
         * Flush any pending resource barriers to the command list.
         *
         * @return The number of resource barriers that were flushed to the command list.
         */
        uint32_t FlushPendingResourceBarriers(const Ref<FDX12CommandList>& CommandList);

        /**
         * Flush any (non-pending) resource barriers that have been pushed to the resource state
         * tracker.
         */
        void FlushResourceBarriers(const Ref<FDX12CommandList>& CommandList);

        /**
         * Commit final resource states to the global resource state map.
         * This must be called when the command list is closed.
         */
        void CommitFinalResourceStates();

        /**
         * Reset state tracking. This must be done when the command list is reset.
         */
        void Reset();

        /**
         * The global state must be locked before flushing pending resource barriers
         * and committing the final resource state to the global resource state.
         * This ensures consistency of the global resource state between command list
         * executions.
         */
        static void Lock();

        /**
         * Unlocks the global resource state after the final states have been committed
         * to the global resource state array.
         */
        static void Unlock();

        /**
         * Add a resource with a given state to the global resource state array (map).
         * This should be done when the resource is created for the first time.
         */
        static void AddGlobalResourceState(ID3D12Resource* Resource, D3D12_RESOURCE_STATES State);

        ///**
        // * Remove a resource from the global resource state array (map).
        // * This should only be done when the resource is destroyed.
        // */
        //static void RemoveGlobalResourceState( ID3D12Resource* resource, bool immediate = false );

        ///**
        // * Remove garbage resources.
        // */
        //static void RemoveGarbageResources();
	private:
		using FResourceBarriers = std::vector<D3D12_RESOURCE_BARRIER>;

		FResourceBarriers m_PendingResourceBarriersList;

		FResourceBarriers m_ResourceBarriersList;

		struct FResourcestate
		{
			explicit FResourcestate(D3D12_RESOURCE_STATES InState = D3D12_RESOURCE_STATE_COMMON)
				: State(InState)
			{}
			
			void SetSubResourceState(UINT SubResource, D3D12_RESOURCE_STATES InState)
			{
				if (SubResource == D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES)
				{
					State = InState;
					SubResourceState.clear();
				}
				else
				{
					SubResourceState[SubResource] = InState;
				}
			}

			D3D12_RESOURCE_STATES GetSubresourceState(UINT Subresource) const
			{
				const auto Iter = SubResourceState.find(Subresource);
				if (Iter != SubResourceState.end())
				{
					return Iter->second;
				}
				return State;
			}

			D3D12_RESOURCE_STATES State;
			std::map<UINT, D3D12_RESOURCE_STATES> SubResourceState;
		};
		
		using ResourceStateMap = std::unordered_map<ID3D12Resource*, FResourcestate>;

		ResourceStateMap m_FinalResourceState;

		static ResourceStateMap s_GlobalResourceState;
		
		static std::mutex s_GlobalMutex;
		static bool       s_bLocked;
	};
}