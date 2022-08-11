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
#include "../Base/Resource.h"

namespace Zero
{
	class FResourceStateTracker
	{
	public:
		FResourceStateTracker();
		virtual ~FResourceStateTracker();

		void ResourceBarrier(const D3D12_RESOURCE_BARRIER& barrier);

		void TransitionResource(ID3D12Resource* Resource, D3D12_RESOURCE_STATES StateAfter,
			UINT SubResource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);
		void TransitionResource(const FResource& Resource, D3D12_RESOURCE_STATES StateAfter,
			UINT SubResource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);
	}
}