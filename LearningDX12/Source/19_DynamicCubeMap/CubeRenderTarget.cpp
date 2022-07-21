#include "CubeRenderTarget.h"

FCubeRenderTarget::FCubeRenderTarget(ID3D12Device* InDevice, UINT InWidth, UINT InHeight, DXGI_FORMAT InFormat)
{
	Device = InDevice;

	Width = InWidth;
	Height = InHeight;
	Format = InFormat;

	Viewport = { 0.0f, 0.0f, (float)Width, (float)Height, 0.0f, 1.0f };
	ScissorRect = { 0, 0, (int)Width, (int)Height };

	BuildResource();
}

ID3D12Resource* FCubeRenderTarget::GetResource()
{
	return CubeMapResource.Get();
}

CD3DX12_GPU_DESCRIPTOR_HANDLE FCubeRenderTarget::GetSrv()
{
	return GpuSrv;
}

CD3DX12_CPU_DESCRIPTOR_HANDLE FCubeRenderTarget::GetRtv(int FaceIndex)
{
	return CpuRtv[FaceIndex];
}

D3D12_VIEWPORT FCubeRenderTarget::GetViewport() const
{
	return Viewport;
}

D3D12_RECT FCubeRenderTarget::GetScissorRect() const
{
	return ScissorRect;
}

void FCubeRenderTarget::BuildDescriptors(CD3DX12_CPU_DESCRIPTOR_HANDLE InCpuSrv, CD3DX12_GPU_DESCRIPTOR_HANDLE InGpuSrv, CD3DX12_CPU_DESCRIPTOR_HANDLE InCpuRtv[6])
{
	CpuSrv = InCpuSrv;
	GpuSrv = InGpuSrv;
	for (int i = 0; i < 6; ++i)
	{
		CpuRtv[i] = InCpuRtv[i];
	}
	BuildDescriptors();
}

void FCubeRenderTarget::BuildDescriptors()
{
	D3D12_SHADER_RESOURCE_VIEW_DESC SrvDesc = {};
	SrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	SrvDesc.Format = Format;
	SrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
	SrvDesc.TextureCube.MostDetailedMip = 0;
	SrvDesc.TextureCube.MipLevels = 1;
	SrvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
	
	Device->CreateShaderResourceView(CubeMapResource.Get(), &SrvDesc, CpuSrv);
	
	for (int i = 0; i < 6; ++i)
	{
		D3D12_RENDER_TARGET_VIEW_DESC RtvDesc;
		RtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
		RtvDesc.Format = Format;
		RtvDesc.Texture2DArray.MipSlice = 0;
		RtvDesc.Texture2DArray.PlaneSlice = 0;

		// Render target to ith element.
		RtvDesc.Texture2DArray.FirstArraySlice = i;

		// Only view one element of the array.
		RtvDesc.Texture2DArray.ArraySize = 1;

		// Create RTV to ith cubemap face.
		Device->CreateRenderTargetView(CubeMapResource.Get(), &RtvDesc, CpuRtv[i]);
	}
}

void FCubeRenderTarget::BuildResource()
{
	// Note, compressed formats cannot be used for UAV.  We get error like:
	// ERROR: ID3D11Device::CreateTexture2D: The format (0x4d, BC3_UNORM) 
	// cannot be bound as an UnorderedAccessView, or cast to a format that
	// could be bound as an UnorderedAccessView.  Therefore this format 
	// does not support D3D11_BIND_UNORDERED_ACCESS.	
	D3D12_RESOURCE_DESC TexDesc;
	ZeroMemory(&TexDesc, sizeof(D3D12_RESOURCE_DESC));
	TexDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	TexDesc.Alignment = 0;
	TexDesc.Width = Width;
	TexDesc.Height = Height;
	TexDesc.DepthOrArraySize = 6;
	TexDesc.MipLevels = 1;
	TexDesc.Format = Format;
	TexDesc.SampleDesc.Count = 1;
	TexDesc.SampleDesc.Quality = 0;
	TexDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	TexDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	
	ThrowIfFailed(Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&TexDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&CubeMapResource)));
}

void FCubeRenderTarget::OnResize(UINT NewWidth, UINT NewHeight)
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

