#pragma once
#include "Core.h"
#include "../Common/DX12Header.h"
#include "Resource.h"

namespace Zero
{
	class IBuffer : public IResource
	{
	public:
		IBuffer(FDX12Device& m_Device);
		IBuffer(FDX12Device& m_Device, const D3D12_RESOURCE_DESC& ResDesc);
		IBuffer(FDX12Device& m_Device, Microsoft::WRL::ComPtr<ID3D12Resource> Resource);
	};
}

