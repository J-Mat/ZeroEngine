//***************************************************************************************
// CubeRenderTarget.h by Frank Luna (C) 2015 All Rights Reserved.
//***************************************************************************************

#pragma once

#include "d3dUtil.h"

class FShadowMapRenderTarget
{
public:
	FShadowMapRenderTarget(ID3D12Device* InDevice,
		UINT Width, UINT Height);

	FShadowMapRenderTarget(const FShadowMapRenderTarget& rhs) = delete;
	FShadowMapRenderTarget& operator=(const FShadowMapRenderTarget& rhs) = delete;
	~FShadowMapRenderTarget() = default;

	ID3D12Resource* GetResource();
	CD3DX12_CPU_DESCRIPTOR_HANDLE GetDsv();

	D3D12_VIEWPORT GetViewport()const;
	D3D12_RECT GetScissorRect()const;

	UINT GetWidth()const { return Width; };
	UINT GetHeight()const { return Height; };
	
	void BuildDescriptors(
		CD3DX12_CPU_DESCRIPTOR_HANDLE InCpuSrv,
		CD3DX12_GPU_DESCRIPTOR_HANDLE InGpuSrv,
		CD3DX12_CPU_DESCRIPTOR_HANDLE InCpuDsv);
	
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
	DXGI_FORMAT Format = DXGI_FORMAT_R24G8_TYPELESS;
	
	CD3DX12_CPU_DESCRIPTOR_HANDLE CpuSrv;
	CD3DX12_GPU_DESCRIPTOR_HANDLE GpuSrv;
	CD3DX12_CPU_DESCRIPTOR_HANDLE CpuDsv;

	Microsoft::WRL::ComPtr<ID3D12Resource> ShadowMapResource = nullptr;
};
