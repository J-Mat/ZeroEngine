#pragma once
#include <optional>
#include "EnumUtil.h"

namespace Zero
{
	enum class EResourceFormat
	{
		UNKNOWN,
		R32G32B32A32_FLOAT,
		R32G32B32A32_UINT,
		R32G32B32A32_SINT,
		R32G32B32_FLOAT,
		R32G32B32_UINT,
		R32G32B32_SINT,
		R16G16B16A16_FLOAT,
		R16G16B16A16_UNORM,
		R16G16B16A16_UINT,
		R16G16B16A16_SNORM,
		R16G16B16A16_SINT,
		R32G32_FLOAT,
		R32G32_UINT,
		R32G32_SINT,
		R32G8X24_TYPELESS,
		D32_FLOAT_S8X24_UINT,
		R10G10B10A2_UNORM,
		R10G10B10A2_UINT,
		R11G11B10_FLOAT,
		R8G8B8A8_UNORM,
		R8G8B8A8_UNORM_SRGB,
		R8G8B8A8_UINT,
		R8G8B8A8_SNORM,
		R8G8B8A8_SINT,
		B8G8R8A8_UNORM,
		B8G8R8A8_UNORM_SRGB,
		R16G16_FLOAT,
		R16G16_UNORM,
		R16G16_UINT,
		R16G16_SNORM,
		R16G16_SINT,
		R32_TYPELESS,
		D32_FLOAT,
		R32_FLOAT,
		R32_UINT,
		R32_SINT,
		R24G8_TYPELESS,
		D24_UNORM_S8_UINT,
		R8G8_UNORM,
		R8G8_UINT,
		R8G8_SNORM,
		R8G8_SINT,
		R16_TYPELESS,
		R16_FLOAT,
		D16_UNORM,
		R16_UNORM,
		R16_UINT,
		R16_SNORM,
		R16_SINT,
		R8_UNORM,
		R8_UINT,
		R8_SNORM,
		R8_SINT,
		BC1_UNORM,
		BC1_UNORM_SRGB,
		BC2_UNORM,
		BC2_UNORM_SRGB,
		BC3_UNORM,
		BC3_UNORM_SRGB,
		BC4_UNORM,
		BC4_SNORM,
		BC5_UNORM,
		BC5_SNORM,
		BC6H_UF16,
		BC6H_SF16,
		BC7_UNORM,
		BC7_UNORM_SRGB
	};

	enum class ESubresourceType : uint8_t
	{
		SubresourceType_SRV,
		SubresourceType_UAV,
		SubresourceType_RTV,
		SubresourceType_DSV,
		SubresourceType_Invalid
	};

	enum class EResourceBindFlag : uint32_t
	{
		None = 0,
		ShaderResource = 1 << 0,
		RenderTarget = 1 << 1,
		DepthStencil = 1 << 2,
		UnorderedAccess = 1 << 3,
	};
	DEFINE_ENUM_BIT_OPERATORS(EResourceBindFlag);

	enum class EResourceUsage : uint8_t
	{
		Default,
		Upload,
		Readback
	};
	
	enum class EResourceState : uint64_t
	{
		Common = 0,
		VertexAndConstantBuffer = 0x1,
		IndexBuffer = 0x2,
		RenderTarget = 0x4,
		UnorderedAccess = 0x8,
		DepthWrite = 0x10,
		DepthRead = 0x20,
		NonPixelShaderResource = 0x40,
		PixelShaderResource = 0x80,
		AllShaderResource = PixelShaderResource | NonPixelShaderResource,
		IndirectArgument = 0x100,
		CopyDest = 0x200,
		CopySource = 0x400,
		RaytracingAccelerationStructure = 0x800,
		GenericRead = VertexAndConstantBuffer | IndexBuffer | DepthRead | NonPixelShaderResource | PixelShaderResource | IndirectArgument | CopySource,
	};
	DEFINE_ENUM_BIT_OPERATORS(EResourceState);


	enum class EBufferMiscFlag : uint32_t
	{
		None,
		IndirectArgs = 1 << 0,
		BufferRaw = 1 << 1,
		BufferStructured = 1 << 2,
		ConstantBuffer = 1 << 3,
		VertexBuffer = 1 << 4,
		IndexBuffer = 1 << 5,
		AccelStruct = 1 << 6
	};
	DEFINE_ENUM_BIT_OPERATORS(EBufferMiscFlag);

	inline constexpr bool IsReadState(EResourceState State)
	{
		return HasAnyFlag(State, EResourceState::GenericRead);
	}
	inline constexpr bool IsWriteState(EResourceState State)
	{
		return HasAnyFlag(State, EResourceState::RenderTarget | EResourceState::UnorderedAccess | EResourceState::DepthWrite | EResourceState::CopyDest);
	}
}