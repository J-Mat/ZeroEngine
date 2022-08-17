#pragma once
#include "Core.h"
#include "Common/DX12Header.h"
#include "Render/RHI/Texture.h"
#include "Base/Resource.h"
#include "Render/Moudule/Image/Image.h"

namespace Zero
{
    class FDescriptorAllocation;
	class FDX12Texture :public FTexture2D, public IResource
	{
	public:
		FDX12Texture(FDX12Device& Device, const D3D12_RESOURCE_DESC& resourceDesc, const D3D12_CLEAR_VALUE* clearValue = nullptr);
		FDX12Texture(FDX12Device& Device, Ref<FImage> ImageData);

		void CreateViews();

        bool CheckSRVSupport() const
        {
            return CheckFormatSupport(D3D12_FORMAT_SUPPORT1_SHADER_SAMPLE);
        }

        bool CheckRTVSupport() const
        {
            return CheckFormatSupport(D3D12_FORMAT_SUPPORT1_RENDER_TARGET);
        }

        bool CheckUAVSupport() const
        {
            return CheckFormatSupport(D3D12_FORMAT_SUPPORT1_TYPED_UNORDERED_ACCESS_VIEW) &&
                CheckFormatSupport(D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD) &&
                CheckFormatSupport(D3D12_FORMAT_SUPPORT2_UAV_TYPED_STORE);
        }

        bool CheckDSVSupport() const
        {
            return CheckFormatSupport(D3D12_FORMAT_SUPPORT1_DEPTH_STENCIL);
        }
	private:
		FDX12Device& m_Device;
           
        FDescriptorAllocation m_RenderTargetView;
	};
}