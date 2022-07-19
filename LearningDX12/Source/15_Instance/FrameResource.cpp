#include "FrameResource.h"

FrameResource::FrameResource(ID3D12Device* device, UINT passCount, UINT maxInstanceCount, UINT materialCount)
{
    ThrowIfFailed(device->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(CmdListAlloc.GetAddressOf())));

    PassCB = std::make_unique<TUploadBuffer<PassConstants>>(device, passCount, true);
    MaterialBuffer = std::make_unique<TUploadBuffer<MaterialData>>(device, materialCount, false);
	InstanceBuffer = std::make_unique<TUploadBuffer<InstanceData>>(device, maxInstanceCount, false);
}

FrameResource::~FrameResource()
{

}