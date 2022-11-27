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
				{ECullMode::CULL_MODE_FRONT, D3D12_CULL_MODE::D3D12_CULL_MODE_FRONT},
				{ECullMode::CULL_MODE_BACK, D3D12_CULL_MODE::D3D12_CULL_MODE_BACK}
			};
			return Map[CullMode];
		}
	};
}

using namespace Microsoft::WRL;