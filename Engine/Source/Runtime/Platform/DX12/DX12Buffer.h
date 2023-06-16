#pragma once

#include "Core.h"
#include "Common/DX12Header.h"
#include "Render/RHI/Buffer.h"
#include "./MemoryManage/Resource/Resource.h"
#include "ResourceView/DX12ShaderResourceView.h"
#include "ResourceView/DX12UnorderedAccessResourceView.h"

namespace Zero
{
	class FResourceView;
	class FDX12Buffer : public FBuffer
	{
	public:
		FDX12Buffer(const FBufferDesc& Desc, void* Data = 0, bool bCreateTextureView = true);
		virtual void* Map() const override { return m_ResourceLocation.m_MappedAddress; }
		virtual void Update(FCommandListHandle CommandListHandle, void const* SrcData, size_t DataSize, size_t Offset = 0) override;
		D3D12_GPU_VIRTUAL_ADDRESS GetGPUAddress();
		//D3D12_GPU_VIRTUAL_ADDRESS GetGPUAddress() { return m_D3DResource->GetGPUVirtualAddress(); }
		ComPtr<ID3D12Resource> GetResource() {return m_ResourceLocation.GetResource()->GetD3DResource(); }
		virtual void* GetNative() override { return m_ResourceLocation.GetResource()->GetD3DResource().Get(); };
		void CreateViews();
		FResourceView* GetSRV();
		FResourceView* GetUAV();

		virtual void CreateSRV(const FBufferSubresourceDesc& Descs) override;
        virtual void CreateUAV(const FBufferSubresourceDesc& Descs) override;

        virtual void ReleaseSRVs() override;
		virtual void ReleaseUAVs() override;
	private:
		FResourceLocation m_ResourceLocation;
		ComPtr<ID3D12Resource> m_D3DResource;
		Scope<FDX12ShaderResourceView> m_Srv;
		Scope<FDX12UnorderedAccessResourceView> m_Uav;
	};
}