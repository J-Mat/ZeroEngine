/*
 * @Author: mikey.zhaopeng 
 * @Date: 2022-05-24 17:07:58 
 * @Last Modified by: J_Mat
 * @Last Modified time: 2022-06-04 18:42:53
 */
#include "FrameResource.h"

FFrameResource::FFrameResource(ID3D12Device* Device, UINT PassCount, UINT ObjectCount, UINT MaterialCount)
{
	ThrowIfFailed(
		Device->CreateCommandAllocator(	
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(CommandListAllocator.GetAddressOf())	
		)
	);
	
	PassCB = std::make_unique<TUploadBuffer<FPassConstants>>(Device, PassCount, true);
 	MaterialCB = std::make_unique<TUploadBuffer<FMaterialConstants>>(Device, MaterialCount, true);
	ObjectCB = std::make_unique<TUploadBuffer<FObjectConstants>>(Device, ObjectCount, true);
}

FFrameResource::~FFrameResource()
{
	
}
