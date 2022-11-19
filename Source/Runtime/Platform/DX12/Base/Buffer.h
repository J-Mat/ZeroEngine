#pragma once
#include "Core.h"
#include "../Common/DX12Header.h"
#include "Resource.h"
#include "../DX12Device.h"

namespace Zero
{
	class IBuffer : public IResource
	{
	public:
		IBuffer();
		IBuffer(const D3D12_RESOURCE_DESC& ResDesc);
		IBuffer(Microsoft::WRL::ComPtr<ID3D12Resource> Resource);
	};
}

