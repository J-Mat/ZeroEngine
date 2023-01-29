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
    
    struct FDX12TextureSettings
    {
        D3D12_RESOURCE_DESC Desc;
		DXGI_FORMAT SRVFormat = DXGI_FORMAT_UNKNOWN;
		DXGI_FORMAT RTVFormat = DXGI_FORMAT_UNKNOWN;
		DXGI_FORMAT DSVFormat = DXGI_FORMAT_UNKNOWN;
		DXGI_FORMAT UAVFormat = DXGI_FORMAT_UNKNOWN;
    };

	class FDX12Texture2D :public FTexture2D, public FResource, public std::enable_shared_from_this<FDX12Texture2D>
	{
        friend class FDX12RenderTarget2D;
	public:
		FDX12Texture2D(const std::string& TextureName, const FDX12TextureSettings& TextureSettings, const D3D12_CLEAR_VALUE* ClearValue = nullptr);
        FDX12Texture2D(const std::string& TextureName, Ref<FImage> ImageData, bool bNeedMipMap = true);
        FDX12Texture2D(const std::string& TextureName, ComPtr<ID3D12Resource> Resource, uint32_t Width, uint32_t Height, const D3D12_CLEAR_VALUE* ClearValue = nullptr);


        static DXGI_FORMAT GetUAVCompatableFormat(DXGI_FORMAT Format);
        static bool IsSRGBFormat(DXGI_FORMAT Format);
        static DXGI_FORMAT GetSRGBFormat(DXGI_FORMAT Format);

        virtual ZMath::uvec2 GetSize() 
        {
            const auto& Desc = GetD3D12ResourceDesc();
            return ZMath::uvec2(Desc.Width, Desc.Height);
        };

        virtual void Resize(uint32_t Width, uint32_t Height, uint32_t DepthOrArraySize);

        virtual void RegistGuiShaderResource() override;
        virtual UINT64 GetGuiShaderReseource() override;

        D3D12_UNORDERED_ACCESS_VIEW_DESC GetUAVDesc(const D3D12_RESOURCE_DESC& ResDesc, UINT MipSlice, UINT ArraySlice = 0, UINT PlaneSlice = 0);

		void CreateViews();

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
		FDescriptorAllocation m_RenderTargetView;
		FDescriptorAllocation m_DepthStencilView;
		FDescriptorAllocation m_ShaderResourceView;
		FDescriptorAllocation m_UnorderedAccessView;
        FLightDescrptorAllocation m_GuiAllocation;
        bool m_bHasGuiResource = false;
    private:
        DXGI_FORMAT m_SRVFormat = DXGI_FORMAT_UNKNOWN;
        DXGI_FORMAT m_RTVFormat = DXGI_FORMAT_UNKNOWN;
        DXGI_FORMAT m_DSVFormat = DXGI_FORMAT_UNKNOWN;
        DXGI_FORMAT m_UAVFormat = DXGI_FORMAT_UNKNOWN;
	};
}