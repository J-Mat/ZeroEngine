#include "ShadowMapRenderTarget.h"

FShadowMapRenderTarget::FShadowMapRenderTarget(ID3D12Device* InDevice, UINT InWidth, UINT InHeight)
{
	Device = InDevice;

	Width = InWidth;
	Height = InHeight;

	Viewport = { 0.0f, 0.0f, (float)Width, (float)Height, 0.0f, 1.0f };
	ScissorRect = { 0, 0, (int)Width, (int)Height };

	BuildResource();
}

ID3D12Resource* FShadowMapRenderTarget::GetResource()
{
	return ShadowMapResource.Get();
}

CD3DX12_CPU_DESCRIPTOR_HANDLE FShadowMapRenderTarget::GetDsv()
{
	return CpuDsv;
}

D3D12_VIEWPORT FShadowMapRenderTarget::GetViewport() const
{
	return Viewport;
}

D3D12_RECT FShadowMapRenderTarget::GetScissorRect() const
{
	return ScissorRect;
}

void FShadowMapRenderTarget::BuildDescriptors(CD3DX12_CPU_DESCRIPTOR_HANDLE InCpuSrv, CD3DX12_GPU_DESCRIPTOR_HANDLE InGpuSrv, CD3DX12_CPU_DESCRIPTOR_HANDLE InCpuDsv)
{
	CpuSrv = InCpuSrv;
	GpuSrv = InGpuSrv;
	CpuDsv = InCpuDsv;
	BuildDescriptors();
}

void FShadowMapRenderTarget::BuildDescriptors()
{
	// Create SRV to resource so we can sample the shadow map in a shader program.
	D3D12_SHADER_RESOURCE_VIEW_DESC SrvDesc = {};
	SrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	SrvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	SrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	SrvDesc.Texture2D.MostDetailedMip = 0;
	SrvDesc.Texture2D.MipLevels = 1;
	SrvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	SrvDesc.Texture2D.PlaneSlice = 0;
	Device->CreateShaderResourceView(ShadowMapResource.Get(), &SrvDesc, CpuSrv);

	// Create DSV to resource so we can render to the shadow map.
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.Texture2D.MipSlice = 0;
	Device->CreateDepthStencilView(ShadowMapResource.Get(), &dsvDesc, CpuDsv);
}


void FShadowMapRenderTarget::BuildResource()
{
	D3D12_RESOURCE_DESC TexDesc;
	ZeroMemory(&TexDesc, sizeof(D3D12_RESOURCE_DESC));
	TexDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	TexDesc.Alignment = 0;
	TexDesc.Width = Width;
	TexDesc.Height = Height;
	TexDesc.DepthOrArraySize = 1;
	TexDesc.MipLevels = 1;
	TexDesc.Format = Format;
	TexDesc.SampleDesc.Count = 1;
	TexDesc.SampleDesc.Quality = 0;
	TexDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	TexDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE OptClear;
	OptClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	OptClear.DepthStencil.Depth = 1.0f;
	OptClear.DepthStencil.Stencil = 0;

	ThrowIfFailed(Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&TexDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		&OptClear,
		IID_PPV_ARGS(&ShadowMapResource)));
}

void FShadowMapRenderTarget::OnResize(UINT NewWidth, UINT NewHeight)
{
	if ((Width != NewWidth) || (Height != NewHeight))
	{
		Width = NewWidth;
		Height = NewHeight;

		BuildResource();

		// New resource, so we need new descriptors to that resource.
		BuildDescriptors();
	}
}

