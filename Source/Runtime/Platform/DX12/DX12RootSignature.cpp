#include "DX12RootSignature.h"
#include "DX12Device.h"

namespace Zero
{
	FDX12RootSignature::FDX12RootSignature(const D3D12_ROOT_SIGNATURE_DESC& RootSignatureDesc)
		: m_RootSignatureDesc{}
		, m_NumDescriptorsPerTable{ 0 }
		, m_SamplerTableBitMask(0)
		, m_DescriptorTableBitMask(0)
	{
		SetRootSignatureDesc(RootSignatureDesc);
	}

	FDX12RootSignature::~FDX12RootSignature()
	{
	}

	uint32_t FDX12RootSignature::GetNumDescriptors(uint32_t RootIndex)
	{
		CORE_ASSERT(RootIndex < 32, "RootSignature num can not exceed 32!");
		return m_NumDescriptorsPerTable[RootIndex];
	}

	void FDX12RootSignature::Destroy()
	{
		for (UINT i = 0; i < m_RootSignatureDesc.NumParameters; ++i)
		{
			const D3D12_ROOT_PARAMETER& RootParameter = m_RootSignatureDesc.pParameters[i];
			if (RootParameter.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
			{
				delete[] RootParameter.DescriptorTable.pDescriptorRanges;
			}
		}

		delete[] m_RootSignatureDesc.pParameters;
		m_RootSignatureDesc.pParameters = nullptr;
		m_RootSignatureDesc.NumParameters = 0;

		delete[] m_RootSignatureDesc.pStaticSamplers;
		m_RootSignatureDesc.pStaticSamplers = nullptr;
		m_RootSignatureDesc.NumStaticSamplers = 0;

		m_DescriptorTableBitMask = 0;
		m_SamplerTableBitMask = 0;

		memset(m_NumDescriptorsPerTable, 0, sizeof(m_NumDescriptorsPerTable));
	}

	void FDX12RootSignature::SetRootSignatureDesc(const D3D12_ROOT_SIGNATURE_DESC& RootSignatureDesc)
	{
		// Make sure any previously allocated root signature description is cleaned
		// up first.
		Destroy();

		UINT NumParameters = RootSignatureDesc.NumParameters;
		D3D12_ROOT_PARAMETER* pParameters = NumParameters > 0 ? new D3D12_ROOT_PARAMETER[NumParameters] : nullptr;
		
		for (UINT i = 0; i < NumParameters; ++i)
		{
			const D3D12_ROOT_PARAMETER& RootParameter = RootSignatureDesc.pParameters[i];
			pParameters[i] = RootParameter;

			if (RootParameter.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
			{
				UINT NumDescriptorRanges = RootParameter.DescriptorTable.NumDescriptorRanges;
				D3D12_DESCRIPTOR_RANGE* pDescirptorRanges = 
					NumDescriptorRanges > 0 ? new D3D12_DESCRIPTOR_RANGE[NumDescriptorRanges] : nullptr;
				
				memcpy(pDescirptorRanges, RootParameter.DescriptorTable.pDescriptorRanges,
					sizeof(D3D12_DESCRIPTOR_RANGE) * NumDescriptorRanges);
				
				pParameters[i].DescriptorTable.NumDescriptorRanges = NumDescriptorRanges;
				pParameters[i].DescriptorTable.pDescriptorRanges = pDescirptorRanges;

				if (NumDescriptorRanges > 0)
				{
					switch (pDescirptorRanges[0].RangeType)
					{
					case D3D12_DESCRIPTOR_RANGE_TYPE_SRV:
					case D3D12_DESCRIPTOR_RANGE_TYPE_UAV:
					case D3D12_DESCRIPTOR_RANGE_TYPE_CBV:
						m_DescriptorTableBitMask |= (1 << i);
					case D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER:
						m_SamplerTableBitMask |= (1 << i);
					}
				}

				// Count the number of descriptors in the descriptor table.  
				for (UINT j = 0; j < NumDescriptorRanges; ++j)
				{
					m_NumDescriptorsPerTable[i] += pDescirptorRanges[j].NumDescriptors;
				}
			}
		}

		m_RootSignatureDesc.NumParameters = NumParameters;
		m_RootSignatureDesc.pParameters = pParameters;
		
		UINT NumStaticSamplers = RootSignatureDesc.NumStaticSamplers;
		
		D3D12_STATIC_SAMPLER_DESC* pStaticSamplers =
			NumStaticSamplers > 0 ? new D3D12_STATIC_SAMPLER_DESC[NumStaticSamplers] : nullptr;
	
		if (pStaticSamplers)
		{
			memcpy(pStaticSamplers, RootSignatureDesc.pStaticSamplers, sizeof(D3D12_STATIC_SAMPLER_DESC) * NumStaticSamplers);
		}
		
		m_RootSignatureDesc.NumStaticSamplers = NumStaticSamplers;
		m_RootSignatureDesc.pStaticSamplers = pStaticSamplers;

		D3D12_ROOT_SIGNATURE_FLAGS Flags = RootSignatureDesc.Flags;
		m_RootSignatureDesc.Flags = Flags;
		
		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC VersionRootSignatureDesc;
		VersionRootSignatureDesc.Init_1_0(NumParameters, pParameters, NumStaticSamplers, pStaticSamplers, Flags);
		
		D3D_ROOT_SIGNATURE_VERSION HighestVersion = FDX12Device::Get()->GetHighestRootSignatureVersion();

		// Serialize the root signature.
		ComPtr<ID3DBlob> RootSignatureBlob;
		ComPtr<ID3DBlob> ErrorBlob;
		ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&VersionRootSignatureDesc, HighestVersion, &RootSignatureBlob, &ErrorBlob));

		FDX12Device::Get()->GetDevice()->CreateRootSignature(0, RootSignatureBlob->GetBufferPointer(), RootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature));
	}

	uint32_t FDX12RootSignature::GetDescriptorTableBitMask(D3D12_DESCRIPTOR_HEAP_TYPE DescriptorHeapType) const
	{
		switch (DescriptorHeapType)
		{
		case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
			return m_DescriptorTableBitMask;
		case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER:
			return m_SamplerTableBitMask;
		}
		return 0;
	}
}
