//***************************************************************************************
// CubeRenderTarget.h by Frank Luna (C) 2015 All Rights Reserved.
//***************************************************************************************

#pragma once

#include "d3dUtil.h"

enum class UCubeMapFace : int
{
	PositiveX = 0,
	NegativeX = 1,
	PositiveY = 2,
	NegativeY = 3,
	PositiveZ = 4,
	NegativeZ = 5
};

class FCubeRenderTarget
{
	FCubeRenderTarget(ID3D12Device* InDevice,
		UINT Width, UINT Height,
		DXGI_FORMAT Format);

	FCubeRenderTarget(const FCubeRenderTarget& rhs) = delete;
	FCubeRenderTarget& operator=(const FCubeRenderTarget& rhs) = delete;
	~FCubeRenderTarget() = default;


	ID3D12Resource* GetResource();
	CD3DX12_GPU_DESCRIPTOR_HANDLE GetSrv();
	CD3DX12_CPU_DESCRIPTOR_HANDLE GetRtv(int faceIndex);

	D3D12_VIEWPORT GetViewport()const;
	D3D12_RECT GetScissorRect()const;
	
	void BuildDescriptors(
		CD3DX12_CPU_DESCRIPTOR_HANDLE InCpuSrv,
		CD3DX12_GPU_DESCRIPTOR_HANDLE InGpuSrv,
		CD3DX12_CPU_DESCRIPTOR_HANDLE InCpuRtv[6]);
	
	void OnResize(UINT NewWidth, UINT NewHeight);


private:
	void BuildDescriptors();
	void BuildResource();

private: 
	ID3D12Device* Device = nullptr;
	
	D3D12_VIEWPORT Viewport;
	D3D12_RECT ScissorRect;
	
	UINT Width = 0;
	UINT Height = 0;
	DXGI_FORMAT Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	
	CD3DX12_CPU_DESCRIPTOR_HANDLE CpuSrv;
	CD3DX12_GPU_DESCRIPTOR_HANDLE GpuSrv;
	CD3DX12_CPU_DESCRIPTOR_HANDLE CpuRtv[6];

	Microsoft::WRL::ComPtr<ID3D12Resource> CubeMapResource = nullptr;
};
