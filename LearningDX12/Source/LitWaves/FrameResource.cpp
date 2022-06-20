/*
 * @Author: mikey.zhaopeng 
 * @Date: 2022-05-24 17:07:58 
 * @Last Modified by:   mikey.zhaopeng 
 * @Last Modified time: 2022-05-24 17:07:58 
 */
#include "FrameResource.h"

FFrameResource::FFrameResource(ID3D12Device* Device, UINT PassCount, UINT ObjectCount, UINT MaterialCount, UINT WaveVertCount)
{
	ThrowIfFailed(
		Device->CreateCommandAllocator(	
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(CommandListAllocator.GetAddressOf())	
		)
	);
	
	PassCB = std::make_unique<TUploadBuffer<FPassConstants>>(Device, PassCount, true);
	ObjectCB = std::make_unique<TUploadBuffer<FObjectConstants>>(Device, ObjectCount, true);
	MaterialCB = std::make_unique<TUploadBuffer<FMaterialConstants>>(Device, MaterialCount, true);
	
	WavesVB = std::make_unique<TUploadBuffer<FVertex>>(Device, WaveVertCount, false);
}

FFrameResource::~FFrameResource()
{
	
}
