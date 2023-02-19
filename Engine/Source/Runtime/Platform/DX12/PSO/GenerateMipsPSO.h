#pragma once

#include "Core.h"
#include "../Common/DX12Header.h"
#include "../MemoryManage/DescriptorAllocation.h"


namespace Zero
{
	struct alignas(16) FGenerateMipsCB
	{
		uint32_t SrcMipLevel; // Texture level of source mip
		uint32_t NumMipLevels;  // Number of OutMips to write: [1-4]
		uint32_t SrcDimension;  // Width and height of the source texture are even or odd.
		uint32_t bIsSRGB;        // Must apply gamma correction to sRGB textures.
		ZMath::vec2 TexelSize;     // 1.0 / OutMip1.Dimensions
	};


	// I don't use scoped enums (C++11) to avoid the explicit cast that is required to
	// treat these as root indices.
	namespace EGenerateMips
	{
		enum
		{
			GM_GenerateMipsCB,
			GM_SrcMip,
			GM_OutMip,
			GM_NumRootParameters
		};
	}

	class FDX12RootSignature;
	class FGenerateMipsPSO
	{
	public:
		FGenerateMipsPSO();
		ComPtr<ID3D12PipelineState> GetPipelineState() { return m_D3DPipelineState;}
		Ref<FDX12RootSignature> GetRootSignature() { return m_RootSignature; }
		D3D12_CPU_DESCRIPTOR_HANDLE GetDefaultUAV() const
		{
			return m_DefaultUAV.GetDescriptorHandle();
		}
	private:
		Ref<FDX12RootSignature> m_RootSignature;
		ComPtr<ID3D12PipelineState> m_D3DPipelineState;

		FDescriptorAllocation m_DefaultUAV;
	};
}
