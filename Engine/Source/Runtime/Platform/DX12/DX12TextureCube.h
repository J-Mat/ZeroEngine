#pragma once
#include "Core.h"
#include "Common/DX12Header.h"
#include "Render/RHI/Texture.h"
#include "Render/Moudule/Texture/Image.h"
#include "./MemoryManage/Resource/Resource.h"
#include "./MemoryManage/Descriptor/DescriptorAllocation.h"
#include "ResourceView/DX12RenderTargetView.h"
#include "ResourceView/DX12DepthStencilView.h"
#include "ResourceView/DX12ShaderResourceView.h"
#include "ResourceView/DX12UnorderedAccessResourceView.h"

namespace Zero
{
    class FDX12Device;
    class FDescriptorAllocation;

	class FDX12TextureCube :public FTextureCube
	{
	public:
        FDX12TextureCube(const std::string& TextureName, const FTextureDesc& Desc, bool bCreateTextureView = true);
        FDX12TextureCube(const std::string& TextureName, Ref<FImage> ImageData[CUBEMAP_TEXTURE_CNT], bool bRenderDepth = false);


        virtual ZMath::uvec2 GetSize() 
        {
            const auto& Desc = m_ResourceLocation.GetResource()->GetD3D12ResourceDesc();
            return ZMath::uvec2(Desc.Width, Desc.Height);
        };
        
        virtual void Resize(uint32_t Width, uint32_t Height, uint32_t DepthOrArraySize = 6);

		void CreateViews(uint32_t MipLevels = 1);

        virtual void SetName(const std::string& Name) override;

        /**
        * Get the RTV for the texture.
        */
        FResourceView* GetRTV(uint32_t FaceIndex = 0, uint32_t SubResourceIndex = -1) const;

        /**
        * Get the DSV for the texture.
        */
        FResourceView* GetDSV(uint32_t ViewID = 0) const;

        /**
        * Get the default SRV for the texture.
        */
        FResourceView* GetSRV(uint32_t ViewID = 0) const;


		Ref<FDX12Resource> GetResource() { return m_ResourceLocation.GetResource(); }
		virtual void* GetNative() override { return m_ResourceLocation.GetResource()->GetD3DResource().Get(); };

        virtual void MakeSRVs(const std::vector<FTextureSubresourceDesc>& Descs) override;
		virtual void MakeRTVs(const std::vector<FTextureSubresourceDesc>& Descs) override;
		virtual void MakeDSVs(const std::vector<FTextureSubresourceDesc>& Descs) override;
		virtual void MakeUAVs(const std::vector<FTextureSubresourceDesc>& Descs) override;

        virtual void ReleaseSRVs() override;
        virtual void ReleaseRTVs() override;
		virtual void ReleaseDSVs() override;
        virtual void ReleaseUAVs() override;
	private:

        uint32_t m_EachFaceSize = 0;
        bool m_bRenderDepth = false;

        FLightDescrptorAllocation m_GuiAllocation;

		std::vector<Scope<FDX12ShaderResourceView>> m_SRVs;
		std::vector<Scope<FDX12UnorderedAccessResourceView>> m_UAVs;
		std::vector<Scope<FDX12RenderTargetView>> m_RTVs;
		std::vector<Scope<FDX12DepthStencilView>> m_DSVs;

		FResourceLocation m_ResourceLocation;
        bool m_bHasGuiResource = false;
	private:
		DXGI_FORMAT m_SRVFormat = DXGI_FORMAT_UNKNOWN;
		DXGI_FORMAT m_RTVFormat = DXGI_FORMAT_UNKNOWN;
		DXGI_FORMAT m_DSVFormat = DXGI_FORMAT_UNKNOWN;
		DXGI_FORMAT m_UAVFormat = DXGI_FORMAT_UNKNOWN;
	};
}