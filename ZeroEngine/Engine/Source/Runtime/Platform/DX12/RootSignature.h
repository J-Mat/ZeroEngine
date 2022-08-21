#pragma once

#include "Core.h"
#include "Common/DX12Header.h"


namespace Zero
{
	class FDX12Device;
	class FRootSignature
	{
	public:
		FRootSignature(FDX12Device& m_Device, const D3D12_ROOT_SIGNATURE_DESC1& RootSignatureDesc);
		virtual	~FRootSignature();
		friend class std::default_delete<FRootSignature>;
		uint32_t GetNumDescriptors(uint32_t RootIndex);
		void SetRootSignatureDesc(const D3D12_ROOT_SIGNATURE_DESC1& RootSignatureDesc);
		uint32_t GetDescriptorTableBitMask(D3D12_DESCRIPTOR_HEAP_TYPE DescriptorHeapType) const;

		const D3D12_ROOT_SIGNATURE_DESC1& GetRootSignatureDesc() const
		{
			return m_RootSignatureDesc;
		}
		ComPtr<ID3D12RootSignature> GetD3D12RootSignature() const
		{
			return m_RootSignature;
		}
	private:
		void Destroy();

		FDX12Device& m_Device;
		D3D12_ROOT_SIGNATURE_DESC1                  m_RootSignatureDesc;
		ComPtr<ID3D12RootSignature> m_RootSignature;

		// Need to know the number of descriptors per descriptor table.
		// A maximum of 32 descriptor tables are supported (since a 32-bit
		// mask is used to represent the descriptor tables in the root signature.
		uint32_t m_NumDescriptorsPerTable[32];

		// A bit mask that represents the root parameter indices that are
		// descriptor tables for Samplers.
		uint32_t m_SamplerTableBitMask;
		// A bit mask that represents the root parameter indices that are
		// CBV, UAV, and SRV descriptor tables.
		uint32_t m_DescriptorTableBitMask;
	};
}

