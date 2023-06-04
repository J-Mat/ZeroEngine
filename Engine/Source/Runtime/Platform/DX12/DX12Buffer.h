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
		FDX12Buffer(const FBufferDesc& Desc, void* Data = 0, bool bCreateTextureView = true);
		virtual void* Map() override { return m_ResourceLocation.m_MappedAddress; }
		virtual void Update(Ref<FCommandList> CommandList, void const* SrcData, size_t DataSize, size_t Offset = 0) override;
		D3D12_GPU_VIRTUAL_ADDRESS GetGPUAddress();
		//D3D12_GPU_VIRTUAL_ADDRESS GetGPUAddress() { return m_D3DResource->GetGPUVirtualAddress(); }
		ComPtr<ID3D12Resource> GetResource() {return m_ResourceLocation.GetResource()->GetD3DResource(); }
		virtual void* GetNative() override { return m_ResourceLocation.GetResource()->GetD3DResource().Get(); };
		void CreateViews();
		FResourceView* GetSRV();
		FResourceView* GetUAV();

		virtual void MakeSRVs(const std::vector<FBufferSubresourceDesc>& Descs) override;
        virtual void MakeUAVs(const std::vector<FBufferSubresourceDesc>& Descs) override;

        virtual void ReleaseSRVs() override;
		virtual void ReleaseUAVs() override;
	private:
		FResourceLocation m_ResourceLocation;
		ComPtr<ID3D12Resource> m_D3DResource;
	};
}