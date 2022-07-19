#include "FrameResource.h"

FFrameResource::FFrameResource(ID3D12Device* device, UINT passCount, UINT objectCount, UINT materialCount)
{
    ThrowIfFailed(device->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(CmdListAlloc.GetAddressOf())));

    PassCB = std::make_unique<TUploadBuffer<PassConstants>>(device, passCount, true);
	MaterialBuffer = std::make_unique<TUploadBuffer<MaterialData>>(device, materialCount, false);
    ObjectCB = std::make_unique<TUploadBuffer<ObjectConstants>>(device, objectCount, true);
}

FFrameResource::~FFrameResource()
{

}