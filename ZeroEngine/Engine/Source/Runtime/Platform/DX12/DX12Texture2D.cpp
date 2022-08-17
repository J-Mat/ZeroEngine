#include "DX12Texture2D.h"

namespace Zero
{
	FDX12Texture::FDX12Texture(FDX12Device& Device, const D3D12_RESOURCE_DESC& resourceDesc, const D3D12_CLEAR_VALUE* clearValue)
		: IResource(Device)
		, m_Device(Device)
	{
	}

	FDX12Texture::FDX12Texture(FDX12Device& Device, Ref<FImage> ImageData)
		: IResource(Device)
		, m_Device(Device)
	{
		auto Resource = m_Device.GetCommandQueue().GetCommandList()->CreateTextureResource(ImageData);
		SetResource(Resource);
		CreateViews();
	}
	void FDX12Texture::CreateViews()
	{
		if (m_D3DResource)
		{
			auto D3DDevice = m_Device.GetDevice();
			
			CD3DX12_RESOURCE_DESC Desc(m_D3DResource->GetDesc());
			
			if ((Desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET) != 0 && CheckRTVSupport())
			{
			}
		}
	}
}
