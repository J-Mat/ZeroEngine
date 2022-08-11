#include "ResourceStateTracker.h"

namespace Zero
{
	FResourceStateTracker::FResourceStateTracker()
	{
	}

	FResourceStateTracker::~FResourceStateTracker()
	{
	}

	void FResourceStateTracker::ResourceBarrier(const D3D12_RESOURCE_BARRIER& barrier)
	{
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
}
