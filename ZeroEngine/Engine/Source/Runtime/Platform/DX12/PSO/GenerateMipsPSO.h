#pragma once

#include "Core.h"
#include "../Common/DX12Header.h"


namespace Zero
{
	struct alignas(16) FGenerateMipsCB
	{
		uint32_t SrcMipLevel; // Texture level of source mip
		uint32_t NumMipLevels;  // Number of OutMips to write: [1-4]
		uint32_t SrcDimension;  // Width and height of the source texture are even or odd.
		uint32_t IsSRGB;        // Must apply gamma correction to sRGB textures.
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
	private:
		Ref<FDX12RootSignature> m_RootSignature;
	};
}
