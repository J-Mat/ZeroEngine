#pragma once
#include "Core.h"
#include "../Common/DX12Header.h"
#include "Resource.h"

namespace Zero
{
	class FBuffer : public FResource
	{
		FBuffer(FDX12Device& Device, const D3D12_RESOURCE_DESC& ResDesc);
		FBuffer(FDX12Device& Device, Microsoft::WRL::ComPtr<ID3D12Resource> Resource);
	};
}

