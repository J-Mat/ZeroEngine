#pragma once
#include "Core.h"
#include "Common/DX12Header.h"
#include "Render/RHI/Texture.h"
#include "Render/Moudule/Texture/Image.h"
#include "./MemoryManage/Resource/Resource.h"
#include "./MemoryManage/Descriptor/DescriptorAllocation.h"

namespace Zero
{
    class FDX12Device;
    class FDescriptorAllocation;

	class FDX12TextureCubemap :public FTextureCubemap
	{
	public:
        FDX12TextureCubemap(const std::string& TextureName, const FTextureDesc& Desc);
		FDX12TextureCubemap(const std::string& TextureName, const D3D12_RESOURCE_DESC& ResourceDesc, const D3D12_CLEAR_VALUE* FTextureClearValue = nullptr);
        FDX12TextureCubemap(const std::string& TextureName, Ref<FImage> ImageData[CUBEMAP_TEXTURE_CNT], bool bRenderDepth = false);
        FDX12TextureCubemap(ComPtr<ID3D12Resource> Resource, uint32_t Width, uint32_t Height, bool bRenderDepth = false, const D3D12_CLEAR_VALUE* FTextureClearValue = nullptr);


        virtual ZMath::uvec2 GetSize() 
        {
            const auto& Desc = m_ResourceLocation.GetResource()->GetD3D12ResourceDesc();
            return ZMath::uvec2(Desc.Width, Desc.Height);
        };
        
        virtual void Resize(uint32_t Width, uint32_t Height, uint32_t DepthOrArraySize = 6);

		void CreateViews();


        /**
        * Get the RTV for the texture.
        */
        D3D12_CPU_DESCRIPTOR_HANDLE GetRenderTargetView(uint32_t Index) const;

        /**
        * Get the DSV for the texture.
        */
        D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView(uint32_t Index) const;

        /**
        * Get the default SRV for the texture.
        */
        D3D12_CPU_DESCRIPTOR_HANDLE GetShaderResourceView() const;

	private:

        uint32_t m_EachFaceSize = 0;
        bool m_bRenderDepth = false;

		FDescriptorAllocation m_RenderTargetView;
		FDescriptorAllocation m_DepthStencilView;
		FDescriptorAllocation m_ShaderResourceView;
        FLightDescrptorAllocation m_GuiAllocation;

		FResourceLocation m_ResourceLocation;
        bool m_bHasGuiResource = false;
	};
}