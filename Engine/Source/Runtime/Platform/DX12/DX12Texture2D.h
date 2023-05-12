#pragma once
#include "Core.h"
#include "Common/DX12Header.h"
#include "./MemoryManage/Resource/Resource.h"
#include "Render/RHI/Texture.h"
#include "Render/Moudule/Texture/Image.h"
#include "./MemoryManage/Descriptor/DescriptorAllocation.h"
#include "ResourceView/DX12RenderTargetView.h"
#include "ResourceView/DX12DepthStencilView.h"
#include "ResourceView/DX12ShaderResourceView.h"
#include "ResourceView/DX12UnorderedAccessResourceView.h"

namespace Zero
{
    class FDX12Device;
    class FDescriptorAllocation;
    
	class FDX12Texture2D :public FTexture2D
	{
        friend class FDX12RenderTarget2D;
    private: 
        D3D12_CLEAR_VALUE GetClearValue();
        void AttachHeapTypeAndResourceState(D3D12_HEAP_TYPE& D3DHeapType, D3D12_RESOURCE_DESC& ResourceDesc);
	public:
        FDX12Texture2D(const std::string& TextureName, const FTextureDesc& Desc, bool bCreateTextureView = true);
        FDX12Texture2D(const std::string& TextureName, Ref<FImage> ImageData, bool bNeedMipMap = true);
        FDX12Texture2D(const std::string& TextureName, ComPtr<ID3D12Resource> Resource, uint32_t Width, uint32_t Height, const D3D12_CLEAR_VALUE* FTextureClearValue = nullptr);
        virtual ~FDX12Texture2D();

        virtual void SetName(const std::string& Name) override;

        static DXGI_FORMAT GetUAVCompatableFormat(DXGI_FORMAT Format);
        static bool IsSRGBFormat(DXGI_FORMAT Format);
        static DXGI_FORMAT GetSRGBFormat(DXGI_FORMAT Format);
        static DXGI_FORMAT GetSRVFormat(DXGI_FORMAT Format);

        virtual ZMath::uvec2 GetSize() 
        {
            const auto& Desc = m_ResourceLocation.m_UnderlyingResource->GetD3D12ResourceDesc();
            return ZMath::uvec2(Desc.Width, Desc.Height);
        };

        virtual void Resize(uint32_t Width, uint32_t Height, uint32_t DepthOrArraySize);

        virtual UINT64 GetGuiShaderReseource() override;

        D3D12_UNORDERED_ACCESS_VIEW_DESC GetUAVDesc(const D3D12_RESOURCE_DESC& ResDesc, UINT MipSlice, UINT ArraySlice = 0, UINT PlaneSlice = 0);

		void CreateViews();

        virtual void MakeSRVs(const std::vector<FTextureSubresourceDesc>& Descs) override; 
        virtual void MakeRTVs(const std::vector<FTextureSubresourceDesc>& Descs) override; 
        virtual void MakeDSVs(const std::vector<FTextureSubresourceDesc>& Descs) override; 
        virtual void MakeUAVs(const std::vector<FTextureSubresourceDesc>& Descs) override; 
        virtual void GenerateMip() override;

        virtual void ReleaseSRVs()  override;
		virtual void ReleaseRTVs()  override;
		virtual void ReleaseDSVs()  override;
		virtual void ReleaseUAVs()  override;

        /**
        * Get the RTV for the texture.
        */
        FResourceView* GetRTV(uint32_t ViewID = 0) const;

        /**
        * Get the DSV for the texture.
        */
        FResourceView* GetDSV(uint32_t ViewID = 0) const;

        /**
        * Get the default SRV for the texture.
        */
        FResourceView* GetSRV(uint32_t ViewID = 0) const;

        /**
        * Get the UAV for the texture at a specific mip level.
        * Note: Only only supported for 1D and 2D textures.
        */
        FResourceView* GetUAV(uint32_t mip) const;
        
        Ref<FDX12Resource> GetResource() { return m_ResourceLocation.GetResource(); }
		virtual void* GetNative() override { return m_ResourceLocation.GetResource()->GetD3DResource().Get(); };

        virtual void RegistGuiShaderResource() override;
	private:

        FLightDescrptorAllocation m_GuiAllocation;
        bool m_bHasGuiResource = false;
        D3D12_PLACED_SUBRESOURCE_FOOTPRINT m_Footprint{};


		std::vector<Scope<FDX12ShaderResourceView>> m_SRVs;
        std::vector<FTextureSubresourceDesc> m_SRVDescs;

		std::vector<Scope<FDX12UnorderedAccessResourceView>> m_UAVs;
        std::vector<FTextureSubresourceDesc> m_UAVDescs;

        std::vector<Scope<FDX12RenderTargetView>> m_RTVs;
        std::vector<FTextureSubresourceDesc> m_RTVDescs;

		std::vector<Scope<FDX12DepthStencilView>> m_DSVs;
        std::vector<FTextureSubresourceDesc> m_DSVDescs;

		FResourceLocation m_ResourceLocation;

    private:
        DXGI_FORMAT m_SRVFormat = DXGI_FORMAT_UNKNOWN;
        DXGI_FORMAT m_RTVFormat = DXGI_FORMAT_UNKNOWN;
        DXGI_FORMAT m_DSVFormat = DXGI_FORMAT_UNKNOWN;
        DXGI_FORMAT m_UAVFormat = DXGI_FORMAT_UNKNOWN;
	};
}