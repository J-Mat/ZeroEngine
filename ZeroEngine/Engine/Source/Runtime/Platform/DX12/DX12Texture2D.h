#pragma once
#include "Core.h"
#include "Common/DX12Header.h"
#include "Render/RHI/Texture.h"
#include "Base/Resource.h"
#include "Render/Moudule/Image/Image.h"

namespace Zero
{
	class FDX12Texture :public ITexture2D, public IResource
	{
	public:
		FDX12Texture(FDX12Device& Device, const D3D12_RESOURCE_DESC& resourceDesc, const D3D12_CLEAR_VALUE* clearValue = nullptr);
		FDX12Texture(FDX12Device& Device, Ref<FImage> ImageData);
	private:
		FDX12Device& m_Device;
	};


}