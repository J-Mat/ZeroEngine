#pragma once
#include "d3dUtil.h"
#include "MathHelper.h"
#include "UploadBuffer.h"

using namespace DirectX;
using namespace Microsoft::WRL;

struct FObjectConstants
{
	DirectX::XMFLOAT4X4 World = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 TexTransform = MathHelper::Identity4x4();
};

struct FPassConstants
{
	XMFLOAT4X4 View = MathHelper::Identity4x4();
	XMFLOAT4X4 InvView = MathHelper::Identity4x4();
	XMFLOAT4X4 Proj = MathHelper::Identity4x4();
	XMFLOAT4X4 InvProj = MathHelper::Identity4x4();
	XMFLOAT4X4 ViewProj = MathHelper::Identity4x4();
	XMFLOAT4X4 InvViewProj = MathHelper::Identity4x4();
	
	XMFLOAT3 EyePosW = { 0.0f, 0.0f, 0.0f};
	float CBPerObjectPad1 = 0.0f;
	XMFLOAT2 RenderTargetSize = { 0.0f, 0.0f};
	XMFLOAT2 InvRenderTargetSize = { 0.0f, 0.0f};
	float NearZ = 0.0f;
	float FarZ = 0.0f;
	float TotalTime = 0.0f;
	float DeltaTime = 0.0f;
	

	XMFLOAT4 AmbientLight = { 0.0f, 0.0f, 0.0f, 1.0f };
	
	// Indices [0, NUM_DIR_LIGHTS) are directional lights;
    // indices [NUM_DIR_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHTS) are point lights;
    // indices [NUM_DIR_LIGHTS+NUM_POINT_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHT+NUM_SPOT_LIGHTS)
    // are spot lights for a maximum of MaxLights per object.
	FLight Lights[MaxLights];
};


struct FVertex
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
};

// Stores the resources needed for the CPU to build the command lists
// for a frame.  
struct FFrameResource
{
public:
	FFrameResource(ID3D12Device* Device, UINT PassCount, UINT ObjectCount, UINT MaterialCount);
	FFrameResource(const FFrameResource& rhs) = delete;
	FFrameResource& operator=(const FFrameResource& rhs) = delete;
	~FFrameResource();

    // We cannot reset the allocator until the GPU is done processing the commands.
    // So each frame needs their own allocator.
	ComPtr<ID3D12CommandAllocator> CommandListAllocator;
	

	// We cannot reset the allocator until the GPU is done processing the commands.
    // So each frame needs their own allocator.    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CmdListAlloc;
	std::unique_ptr<TUploadBuffer<FPassConstants>> PassCB = nullptr;
	std::unique_ptr<TUploadBuffer<FMaterialConstants>> MaterialCB = nullptr;
	std::unique_ptr<TUploadBuffer<FObjectConstants>> ObjectCB = nullptr;


    // We cannot update a dynamic vertex buffer until the GPU is done processing
    // the commands that reference it.  So each frame needs their own.
    //std::unique_ptr<TUploadBuffer<FVertex>> WavesVB = nullptr;

	UINT64 Fence = 0;
};