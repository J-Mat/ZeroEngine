#pragma once
#include "Core.h"
#include "Common/DX12Header.h"
#include "Render/RHI/Texture.h"
#include "Base/Resource.h"
#include "Render/Moudule/Image/Image.h"
#include "./GPUMemory/DescriptorAllocation.h"

namespace Zero
{
    class FDX12Device;
    class FDescriptorAllocation;
	class FDX12Texture2D :public FTexture2D, public IResource
	{
	public:
		FDX12Texture2D(FDX12Device& Device, const D3D12_RESOURCE_DESC& ResourceDesc, const D3D12_CLEAR_VALUE* ClearValue = nullptr);
		FDX12Texture2D(FDX12Device& Device, Ref<FImage> ImageData);
        FDX12Texture2D(FDX12Device& Device, ComPtr<ID3D12Resource> Resource, const D3D12_CLEAR_VALUE* ClearValue);
        
        virtual void Resize(uint32_t Width, uint32_t Height, uint32_t DepthOrArraySize);

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
		FDX12Device& m_Device;

		FDescriptorAllocation m_RenderTargetView;
		FDescriptorAllocation m_DepthStencilView;
		FDescriptorAllocation m_ShaderResourceView;
		FDescriptorAllocation m_UnorderedAccessView;
	};
}