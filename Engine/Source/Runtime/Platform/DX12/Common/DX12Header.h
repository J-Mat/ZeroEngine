#pragma once
#include "Utils.h"
#include <wrl.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <D3Dcompiler.h>
#include <DirectXColors.h>
#include "d3dx12.h"
#include <comdef.h> 
#include <cstdint>
#include <codecvt>
#include "Render/RenderConfig.h"
#include "Render/RHI/Shader/ShaderData.h"
#include "Render/RHI/Texture.h"
#include "Render/ResourceCommon.h"

namespace Zero
{ 
	inline void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			_com_error err(hr);
			OutputDebugString(err.ErrorMessage());
			std::wstring test = err.ErrorMessage();
			throw std::exception(Utils::WString2String(test).c_str());
		}
	}


	class FDX12Utils
	{
	public:
		static D3D12_PRIMITIVE_TOPOLOGY_TYPE GetTopologyType(EPrimitiveTopologyType TopologyType)
		{
			static std::map<EPrimitiveTopologyType, D3D12_PRIMITIVE_TOPOLOGY_TYPE> Map =
			{
				{EPrimitiveTopologyType::PRIMITIVE_TOPOLOGY_TYPE_POINT, D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT},
				{EPrimitiveTopologyType::PRIMITIVE_TOPOLOGY_TYPE_LINE, D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE},
				{EPrimitiveTopologyType::PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE}
			};
			return Map[TopologyType];
		}

		static D3D12_COMPARISON_FUNC GetComparisonFunc(EComparisonFunc ComparisonFunc)
		{
			static std::map<EComparisonFunc, D3D12_COMPARISON_FUNC> Map =
			{
				{EComparisonFunc::NEVER, D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_NEVER},
				{EComparisonFunc::LESS, D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_LESS},
				{EComparisonFunc::EQUAL, D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_EQUAL},
				{EComparisonFunc::LESS_EQUAL, D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_LESS_EQUAL},
				{EComparisonFunc::GREATER, D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_GREATER},
				{EComparisonFunc::NOT_EQUAL, D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_NOT_EQUAL},
				{EComparisonFunc::GREATER_EQUAL, D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_GREATER_EQUAL},
				{EComparisonFunc::ALWAYS, D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_ALWAYS}
			};
			return Map[ComparisonFunc];
		}

		static D3D12_CULL_MODE GetCullMode(ECullMode CullMode)
		{
			static std::map<ECullMode, D3D12_CULL_MODE> Map =
			{
				{ECullMode::CULL_MODE_NONE, D3D12_CULL_MODE::D3D12_CULL_MODE_NONE},
				{ECullMode::CULL_MODE_FRONT, D3D12_CULL_MODE::D3D12_CULL_MODE_FRONT},
				{ECullMode::CULL_MODE_BACK, D3D12_CULL_MODE::D3D12_CULL_MODE_BACK}
			};
			return Map[CullMode];
		}

		static EShaderDataType GetTypeByName(const std::string& TypeName)
		{
			static std::map<std::string, EShaderDataType> NameTypeMap =
			{
				{"float", Zero::EShaderDataType::Float},
				{"float2", Zero::EShaderDataType::Float2},
				{"float3", Zero::EShaderDataType::Float3},
				{"float4", Zero::EShaderDataType::Float4},
				{"int", Zero::EShaderDataType::Int},
				{"int2", Zero::EShaderDataType::Int2},
				{"int3", Zero::EShaderDataType::Int3},
				{"int4", Zero::EShaderDataType::Int4},
				{"bool", Zero::EShaderDataType::Bool},
				{"float3x3", Zero::EShaderDataType::Mat3},
				{"float4x4", Zero::EShaderDataType::Mat4},

			};
			return NameTypeMap[TypeName];
		}

		static EShaderResourceType GetResourceByDimension(const D3D_SRV_DIMENSION& Dimension)
		{
			static std::map<D3D_SRV_DIMENSION, EShaderResourceType> DimentionResourceType =
			{
				{D3D_SRV_DIMENSION_TEXTURE2D, EShaderResourceType::Texture2D},
				{D3D_SRV_DIMENSION_TEXTURECUBE, EShaderResourceType::Cubemap}
			};
			return DimentionResourceType[Dimension];
		}
		/*
		static DXGI_FORMAT GetTextureFormatByDesc(ETextureFormat Format)
		{
			switch (Format)
			{
			case Zero::ETextureFormat::None:
				return DXGI_FORMAT_UNKNOWN;
			case Zero::ETextureFormat::R8G8B8:
				return DXGI_FORMAT_R8G8_UNORM;
			case Zero::ETextureFormat::R8G8B8A8:
				return DXGI_FORMAT_R8G8B8A8_UNORM;
			case Zero::ETextureFormat::R16G16B16A16:
				return DXGI_FORMAT_R16G16B16A16_UNORM;
			case Zero::ETextureFormat::INT32:
				return DXGI_FORMAT_R32_UINT;
			case Zero::ETextureFormat::R32G32B32A32:
				return DXGI_FORMAT_R32G32B32A32_FLOAT;
			case Zero::ETextureFormat::DEPTH32F:
				return DXGI_FORMAT_D24_UNORM_S8_UINT;
			default:
				break;
			}
			return DXGI_FORMAT_UNKNOWN;
		}
		*/


		static constexpr DXGI_FORMAT ConvertResourceFormat(EResourceFormat _format)
		{
			switch (_format)
			{
			case EResourceFormat::UNKNOWN:
				return DXGI_FORMAT_UNKNOWN;

			case EResourceFormat::R32G32B32A32_FLOAT:
				return DXGI_FORMAT_R32G32B32A32_FLOAT;

			case EResourceFormat::R32G32B32A32_UINT:
				return DXGI_FORMAT_R32G32B32A32_UINT;

			case EResourceFormat::R32G32B32A32_SINT:
				return DXGI_FORMAT_R32G32B32A32_SINT;

			case EResourceFormat::R32G32B32_FLOAT:
				return DXGI_FORMAT_R32G32B32_FLOAT;

			case EResourceFormat::R32G32B32_UINT:
				return DXGI_FORMAT_R32G32B32_UINT;

			case EResourceFormat::R32G32B32_SINT:
				return DXGI_FORMAT_R32G32B32_SINT;

			case EResourceFormat::R16G16B16A16_FLOAT:
				return DXGI_FORMAT_R16G16B16A16_FLOAT;

			case EResourceFormat::R16G16B16A16_UNORM:
				return DXGI_FORMAT_R16G16B16A16_UNORM;

			case EResourceFormat::R16G16B16A16_UINT:
				return DXGI_FORMAT_R16G16B16A16_UINT;

			case EResourceFormat::R16G16B16A16_SNORM:
				return DXGI_FORMAT_R16G16B16A16_SNORM;

			case EResourceFormat::R16G16B16A16_SINT:
				return DXGI_FORMAT_R16G16B16A16_SINT;

			case EResourceFormat::R32G32_FLOAT:
				return DXGI_FORMAT_R32G32_FLOAT;

			case EResourceFormat::R32G32_UINT:
				return DXGI_FORMAT_R32G32_UINT;

			case EResourceFormat::R32G32_SINT:
				return DXGI_FORMAT_R32G32_SINT;

			case EResourceFormat::R32G8X24_TYPELESS:
				return DXGI_FORMAT_R32G8X24_TYPELESS;

			case EResourceFormat::D32_FLOAT_S8X24_UINT:
				return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;

			case EResourceFormat::R10G10B10A2_UNORM:
				return DXGI_FORMAT_R10G10B10A2_UNORM;

			case EResourceFormat::R10G10B10A2_UINT:
				return DXGI_FORMAT_R10G10B10A2_UINT;

			case EResourceFormat::R11G11B10_FLOAT:
				return DXGI_FORMAT_R11G11B10_FLOAT;

			case EResourceFormat::R8G8B8A8_UNORM:
				return DXGI_FORMAT_R8G8B8A8_UNORM;

			case EResourceFormat::R8G8B8A8_UNORM_SRGB:
				return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

			case EResourceFormat::R8G8B8A8_UINT:
				return DXGI_FORMAT_R8G8B8A8_UINT;

			case EResourceFormat::R8G8B8A8_SNORM:
				return DXGI_FORMAT_R8G8B8A8_SNORM;

			case EResourceFormat::R8G8B8A8_SINT:
				return DXGI_FORMAT_R8G8B8A8_SINT;

			case EResourceFormat::R16G16_FLOAT:
				return DXGI_FORMAT_R16G16_FLOAT;

			case EResourceFormat::R16G16_UNORM:
				return DXGI_FORMAT_R16G16_UNORM;

			case EResourceFormat::R16G16_UINT:
				return DXGI_FORMAT_R16G16_UINT;

			case EResourceFormat::R16G16_SNORM:
				return DXGI_FORMAT_R16G16_SNORM;

			case EResourceFormat::R16G16_SINT:
				return DXGI_FORMAT_R16G16_SINT;

			case EResourceFormat::R32_TYPELESS:
				return DXGI_FORMAT_R32_TYPELESS;


			case EResourceFormat::D32_FLOAT:
				return DXGI_FORMAT_D32_FLOAT;

			case EResourceFormat::R32_FLOAT:
				return DXGI_FORMAT_R32_FLOAT;

			case EResourceFormat::R32_UINT:
				return DXGI_FORMAT_R32_UINT;

			case EResourceFormat::R32_SINT:
				return DXGI_FORMAT_R32_SINT;

			case EResourceFormat::R8G8_UNORM:
				return DXGI_FORMAT_R8G8_UNORM;

			case EResourceFormat::R8G8_UINT:
				return DXGI_FORMAT_R8G8_UINT;

			case EResourceFormat::R8G8_SNORM:
				return DXGI_FORMAT_R8G8_SNORM;

			case EResourceFormat::R8G8_SINT:
				return DXGI_FORMAT_R8G8_SINT;

			case EResourceFormat::R16_TYPELESS:
				return DXGI_FORMAT_R16_TYPELESS;

			case EResourceFormat::R16_FLOAT:
				return DXGI_FORMAT_R16_FLOAT;

			case EResourceFormat::D16_UNORM:
				return DXGI_FORMAT_D16_UNORM;

			case EResourceFormat::R16_UNORM:
				return DXGI_FORMAT_R16_UNORM;

			case EResourceFormat::R16_UINT:
				return DXGI_FORMAT_R16_UINT;

			case EResourceFormat::R16_SNORM:
				return DXGI_FORMAT_R16_SNORM;

			case EResourceFormat::R16_SINT:
				return DXGI_FORMAT_R16_SINT;

			case EResourceFormat::R8_UNORM:
				return DXGI_FORMAT_R8_UNORM;

			case EResourceFormat::R8_UINT:
				return DXGI_FORMAT_R8_UINT;

			case EResourceFormat::R8_SNORM:
				return DXGI_FORMAT_R8_SNORM;

			case EResourceFormat::R8_SINT:
				return DXGI_FORMAT_R8_SINT;

			case EResourceFormat::BC1_UNORM:
				return DXGI_FORMAT_BC1_UNORM;

			case EResourceFormat::BC1_UNORM_SRGB:
				return DXGI_FORMAT_BC1_UNORM_SRGB;

			case EResourceFormat::BC2_UNORM:
				return DXGI_FORMAT_BC2_UNORM;

			case EResourceFormat::BC2_UNORM_SRGB:
				return DXGI_FORMAT_BC2_UNORM_SRGB;

			case EResourceFormat::BC3_UNORM:
				return DXGI_FORMAT_BC3_UNORM;

			case EResourceFormat::BC3_UNORM_SRGB:
				return DXGI_FORMAT_BC3_UNORM_SRGB;

			case EResourceFormat::BC4_UNORM:
				return DXGI_FORMAT_BC4_UNORM;

			case EResourceFormat::BC4_SNORM:
				return DXGI_FORMAT_BC4_SNORM;

			case EResourceFormat::BC5_UNORM:
				return DXGI_FORMAT_BC5_UNORM;

			case EResourceFormat::BC5_SNORM:
				return DXGI_FORMAT_BC5_SNORM;

			case EResourceFormat::B8G8R8A8_UNORM:
				return DXGI_FORMAT_B8G8R8A8_UNORM;

			case EResourceFormat::B8G8R8A8_UNORM_SRGB:
				return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;

			case EResourceFormat::BC6H_UF16:
				return DXGI_FORMAT_BC6H_UF16;

			case EResourceFormat::BC6H_SF16:
				return DXGI_FORMAT_BC6H_SF16;

			case EResourceFormat::BC7_UNORM:
				return DXGI_FORMAT_BC7_UNORM;

			case EResourceFormat::BC7_UNORM_SRGB:
				return DXGI_FORMAT_BC7_UNORM_SRGB;

			case EResourceFormat::D24_UNORM_S8_UINT:
				return DXGI_FORMAT_D24_UNORM_S8_UINT;

			case EResourceFormat::R24G8_TYPELESS:
				return DXGI_FORMAT_R24G8_TYPELESS;
			}
			return DXGI_FORMAT_UNKNOWN;
		}

		uint32_t static GetFormatStride(EResourceFormat _format)
		{
			DXGI_FORMAT format = ConvertResourceFormat(_format);
			switch (format)
			{
			case DXGI_FORMAT_BC1_UNORM:
			case DXGI_FORMAT_BC1_UNORM_SRGB:
			case DXGI_FORMAT_BC4_SNORM:
			case DXGI_FORMAT_BC4_UNORM:
				return 8u;
			case DXGI_FORMAT_R32G32B32A32_FLOAT:
			case DXGI_FORMAT_R32G32B32A32_UINT:
			case DXGI_FORMAT_R32G32B32A32_SINT:
			case DXGI_FORMAT_BC2_UNORM:
			case DXGI_FORMAT_BC2_UNORM_SRGB:
			case DXGI_FORMAT_BC3_UNORM:
			case DXGI_FORMAT_BC3_UNORM_SRGB:
			case DXGI_FORMAT_BC5_SNORM:
			case DXGI_FORMAT_BC5_UNORM:
			case DXGI_FORMAT_BC6H_UF16:
			case DXGI_FORMAT_BC6H_SF16:
			case DXGI_FORMAT_BC7_UNORM:
			case DXGI_FORMAT_BC7_UNORM_SRGB:
				return 16u;
			case DXGI_FORMAT_R32G32B32_FLOAT:
			case DXGI_FORMAT_R32G32B32_UINT:
			case DXGI_FORMAT_R32G32B32_SINT:
				return 12u;
			case DXGI_FORMAT_R16G16B16A16_FLOAT:
			case DXGI_FORMAT_R16G16B16A16_UNORM:
			case DXGI_FORMAT_R16G16B16A16_UINT:
			case DXGI_FORMAT_R16G16B16A16_SNORM:
			case DXGI_FORMAT_R16G16B16A16_SINT:
				return 8u;
			case DXGI_FORMAT_R32G32_FLOAT:
			case DXGI_FORMAT_R32G32_UINT:
			case DXGI_FORMAT_R32G32_SINT:
			case DXGI_FORMAT_R32G8X24_TYPELESS:
			case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
				return 8u;
			case DXGI_FORMAT_R10G10B10A2_UNORM:
			case DXGI_FORMAT_R10G10B10A2_UINT:
			case DXGI_FORMAT_R11G11B10_FLOAT:
			case DXGI_FORMAT_R8G8B8A8_UNORM:
			case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
			case DXGI_FORMAT_R8G8B8A8_UINT:
			case DXGI_FORMAT_R8G8B8A8_SNORM:
			case DXGI_FORMAT_R8G8B8A8_SINT:
			case DXGI_FORMAT_B8G8R8A8_UNORM:
			case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
			case DXGI_FORMAT_R16G16_FLOAT:
			case DXGI_FORMAT_R16G16_UNORM:
			case DXGI_FORMAT_R16G16_UINT:
			case DXGI_FORMAT_R16G16_SNORM:
			case DXGI_FORMAT_R16G16_SINT:
			case DXGI_FORMAT_R32_TYPELESS:
			case DXGI_FORMAT_D32_FLOAT:
			case DXGI_FORMAT_R32_FLOAT:
			case DXGI_FORMAT_R32_UINT:
			case DXGI_FORMAT_R32_SINT:
			case DXGI_FORMAT_R24G8_TYPELESS:
			case DXGI_FORMAT_D24_UNORM_S8_UINT:
				return 4u;
			case DXGI_FORMAT_R8G8_UNORM:
			case DXGI_FORMAT_R8G8_UINT:
			case DXGI_FORMAT_R8G8_SNORM:
			case DXGI_FORMAT_R8G8_SINT:
			case DXGI_FORMAT_R16_TYPELESS:
			case DXGI_FORMAT_R16_FLOAT:
			case DXGI_FORMAT_D16_UNORM:
			case DXGI_FORMAT_R16_UNORM:
			case DXGI_FORMAT_R16_UINT:
			case DXGI_FORMAT_R16_SNORM:
			case DXGI_FORMAT_R16_SINT:
				return 2u;
			case DXGI_FORMAT_R8_UNORM:
			case DXGI_FORMAT_R8_UINT:
			case DXGI_FORMAT_R8_SNORM:
			case DXGI_FORMAT_R8_SINT:
				return 1u;
			default:
				break;
			}
			return 16u;
		}

		static DXGI_FORMAT GetSRGBFormat(DXGI_FORMAT Format)
		{
			DXGI_FORMAT SrgbFormat = Format;
			switch (Format)
			{
			case DXGI_FORMAT_R8G8B8A8_UNORM:
				SrgbFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
				break;
			case DXGI_FORMAT_BC1_UNORM:
				SrgbFormat = DXGI_FORMAT_BC1_UNORM_SRGB;
				break;
			case DXGI_FORMAT_BC2_UNORM:
				SrgbFormat = DXGI_FORMAT_BC2_UNORM_SRGB;
				break;
			case DXGI_FORMAT_BC3_UNORM:
				SrgbFormat = DXGI_FORMAT_BC3_UNORM_SRGB;
				break;
			case DXGI_FORMAT_B8G8R8A8_UNORM:
				SrgbFormat = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
				break;
			case DXGI_FORMAT_B8G8R8X8_UNORM:
				SrgbFormat = DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;
				break;
			case DXGI_FORMAT_BC7_UNORM:
				SrgbFormat = DXGI_FORMAT_BC7_UNORM_SRGB;
				break;
			}

			return SrgbFormat;
		}
		static DXGI_FORMAT GetSRVFormat(DXGI_FORMAT Format)
		{
			switch (Format)
			{
			case DXGI_FORMAT_D24_UNORM_S8_UINT:
				return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
			default:
				return DXGI_FORMAT_UNKNOWN;
			}
			return DXGI_FORMAT_UNKNOWN;
		}

		static D3D12_RESOURCE_FLAGS BindFlagsByResourceFlags(EResourceBindFlag BindFlags)
		{
			D3D12_RESOURCE_FLAGS Flags = D3D12_RESOURCE_FLAG_NONE;
			if (HasAnyFlag(BindFlags, EResourceBindFlag::DepthStencil))
			{
				Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

				if (!HasAnyFlag(BindFlags, EResourceBindFlag::ShaderResource))
				{
					Flags |= D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;
				}
			}
			if (HasAnyFlag(BindFlags, EResourceBindFlag::RenderTarget))
			{
				Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
			}
			if (HasAllFlags(BindFlags, EResourceBindFlag::UnorderedAccess))
			{
				Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
			}
			return Flags;
		}

		static constexpr D3D12_RESOURCE_STATES ConvertToD3DResourceState(EResourceState State)
		{
			D3D12_RESOURCE_STATES ApiState = D3D12_RESOURCE_STATE_COMMON;
			if (HasAnyFlag(State, EResourceState::VertexAndConstantBuffer)) ApiState |= D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
			if (HasAnyFlag(State, EResourceState::IndexBuffer)) ApiState |= D3D12_RESOURCE_STATE_INDEX_BUFFER;
			if (HasAnyFlag(State, EResourceState::RenderTarget)) ApiState |= D3D12_RESOURCE_STATE_RENDER_TARGET;
			if (HasAnyFlag(State, EResourceState::UnorderedAccess)) ApiState |= D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
			if (HasAnyFlag(State, EResourceState::DepthWrite)) ApiState |= D3D12_RESOURCE_STATE_DEPTH_WRITE;
			if (HasAnyFlag(State, EResourceState::DepthRead)) ApiState |= D3D12_RESOURCE_STATE_DEPTH_READ;
			if (HasAnyFlag(State, EResourceState::NonPixelShaderResource)) ApiState |= D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
			if (HasAnyFlag(State, EResourceState::PixelShaderResource)) ApiState |= D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
			if (HasAnyFlag(State, EResourceState::IndirectArgument)) ApiState |= D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT;
			if (HasAnyFlag(State, EResourceState::CopyDest)) ApiState |= D3D12_RESOURCE_STATE_COPY_DEST;
			if (HasAnyFlag(State, EResourceState::CopySource)) ApiState |= D3D12_RESOURCE_STATE_COPY_SOURCE;
			if (HasAnyFlag(State, EResourceState::RaytracingAccelerationStructure)) ApiState |= D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
			return ApiState;
		}

		static D3D12_RESOURCE_DESC ConvertResourceDesc(const FTextureDesc& TextureDesc)
		{
			D3D12_RESOURCE_DESC ResourceDesc = 
			{
				.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
				.Alignment = 0,
				.Width = TextureDesc.Width,
				.Height = TextureDesc.Height,
				.DepthOrArraySize = (UINT16)TextureDesc.ArraySize,
				.MipLevels = TextureDesc.MipLevels,
				.Format = FDX12Utils::ConvertResourceFormat(TextureDesc.Format),
				.SampleDesc =
				{
					.Count = TextureDesc.SampleCount,
					.Quality = 0
				},
				.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
				.Flags = FDX12Utils::BindFlagsByResourceFlags(TextureDesc.ResourceBindFlags),
			};
			return ResourceDesc;
		}
	};
}

using namespace Microsoft::WRL;