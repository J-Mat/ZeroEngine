#pragma once
#include "Core.h"
#include "Common/DX12Header.h"
#include "Base/Resource.h"
#include "Render/RHI/Texture.h"
#include "Render/Moudule/Image/Image.h"
#include "./MemoryManage/DescriptorAllocation.h"

namespace Zero
{
    class FDX12Device;
    class FDescriptorAllocation;

	class FDX12TextureCubemap :public FTextureCubemap, public IResource
	{
	public:
        static DXGI_FORMAT GetFormatByDesc(ETextureFormat Format);
		FDX12TextureCubemap(const D3D12_RESOURCE_DESC& ResourceDesc, const D3D12_CLEAR_VALUE* ClearValue = nullptr);
		FDX12TextureCubemap(Ref<FImage> ImageData[CUBEMAP_TEXTURE_CNT]);
        FDX12TextureCubemap(ComPtr<ID3D12Resource> Resource, uint32_t Width, uint32_t Height, const D3D12_CLEAR_VALUE* ClearValue = nullptr);


        virtual ZMath::uvec2 GetSize() 
        {
            const auto& Desc = GetD3D12ResourceDesc();
            return ZMath::uvec2(Desc.Width, Desc.Height);
        };
        
        virtual void Resize(uint32_t Width, uint32_t Height, uint32_t DepthOrArraySize = 6);

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
        D3D12_UNORDERED_ACCESS_VIEW_DESC GetUAVDesc(const D3D12_RESOURCE_DESC& ResDesc, UINT MipSlice, UINT ArraySlice = 0,
            UINT PlaneSlice = 0);

        bool CheckDSVSupport() const
        {
            return CheckFormatSupport(D3D12_FORMAT_SUPPORT1_DEPTH_STENCIL);
        }

        /**
        * Get the RTV for the texture.
        */
        D3D12_CPU_DESCRIPTOR_HANDLE GetRenderTargetView() const;

        /**
        * Get the DSV for the texture.
        */
        D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const;

        /**
        * Get the default SRV for the texture.
        */
        D3D12_CPU_DESCRIPTOR_HANDLE GetShaderResourceView() const;

        /**
        * Get the UAV for the texture at a specific mip level.
        * Note: Only only supported for 1D and 2D textures.
        */
        D3D12_CPU_DESCRIPTOR_HANDLE GetUnorderedAccessView(uint32_t mip) const;
        
	private:
        uint32_t m_EachFaceSize = 0;

		FDescriptorAllocation m_RenderTargetView;
		FDescriptorAllocation m_DepthStencilView;
		FDescriptorAllocation m_ShaderResourceView;
		FDescriptorAllocation m_UnorderedAccessView;
        FLightDescrptorAllocation m_GuiAllocation;
        bool m_bHasGuiResource = false;
	};
}