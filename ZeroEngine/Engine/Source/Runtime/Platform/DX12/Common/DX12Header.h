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
#include "Render/RHI/ShaderData.h"
#include "Render/RHI/Texture.h"

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

	class FDX12RHItConverter
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
	};
}

using namespace Microsoft::WRL;