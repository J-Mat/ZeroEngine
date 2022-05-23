#include "FrameResource.h"

FFrameResource::FrameResource(ID3D12Device* Device, UINT PassCount, UINT ObjectCount, UINT WaveVertCount)
{
	ThrowIfFailed(
		Device->CreateCommandAllocator(	
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(CommandListAllocator.Get())	
		)
	);
	
	PassCB = std::make_unique<TUploadBuffer<FPassConstants>>(Device, PassCount, true);
	ObjectCB = std::make_unique<TUploadBuffer<FObjectConstants>>(Device, ObjectCount, true);
	
	WavesVB = std::make_unique<TUploadBuffer<FVertex>>(Device, WaveVertCount, false);
}

FFrameResource::~FFrameResource()
{
	
}
