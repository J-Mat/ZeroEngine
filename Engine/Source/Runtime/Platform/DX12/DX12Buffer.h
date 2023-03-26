#pragma once

#include "Core.h"
#include "Common/DX12Header.h"
#include "Render/RHI/Buffer.h"
#include "./MemoryManage/Resource/Resource.h"

namespace Zero
{
	class FDX12Buffer : public FBuffer
	{
	public:
		FDX12Buffer(const FBufferDesc& Desc, void* Data = 0);
		void* GetMappedData() { return m_ResourceLocation.m_MappedAddress; }
		D3D12_GPU_VIRTUAL_ADDRESS GetGPUAddress();
		//D3D12_GPU_VIRTUAL_ADDRESS GetGPUAddress() { return m_D3DResource->GetGPUVirtualAddress(); }
		ComPtr<ID3D12Resource> GetResource() {return m_ResourceLocation.GetResource()->GetD3DResource(); }
		virtual void* GetNative() override { return m_ResourceLocation.GetResource()->GetD3DResource().Get(); };
	private:
		FResourceLocation m_ResourceLocation;
		ComPtr<ID3D12Resource> m_D3DResource;
	};
}