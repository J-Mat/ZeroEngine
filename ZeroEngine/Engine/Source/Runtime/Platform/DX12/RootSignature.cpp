#include "RootSignature.h"

namespace Zero
{
	FRootSignature::FRootSignature(FDX12Device& Device, const D3D12_ROOT_SIGNATURE_DESC1& RootSignatureDesc)
		: m_Device(Device)
		, m_RootSignatureDesc{}
		, m_NumDescriptorsPerTable{ 0 }
		, m_SamplerTableBitMask(0)
		, m_DescriptorTableBitMask(0)
	{
	}
	FRootSignature::~FRootSignature()
	{
	}
	void FRootSignature::Destroy()
	{
	}
	void FRootSignature::SetRootSignatureDesc(const D3D12_ROOT_SIGNATURE_DESC1& RootSignatureDesc)
	{
		// Make sure any previously allocated root signature description is cleaned
		// up first.
		Destroy();

		UINT NumParameters = RootSignatureDesc.NumParameters;
		D3D12_ROOT_PARAMETER1* pParameters = NumParameters > 0 ? new D3D12_ROOT_PARAMETER1[NumParameters] : nullptr;
		
		if (UINT i = 0; i < NumParameters; ++i)
		{
			const D3D12_ROOT_PARAMETER1& RootParameter = RootSignatureDesc.pParameters[i];
			pParameters[i] = RootParameter;

			if (RootParameter.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
			{
				UINT NumDescriptorRanges = RootParameter.DescriptorTable.NumDescriptorRanges;
				D3D12_DESCRIPTOR_RANGE1* pDescirptorRanges = 
					NumDescriptorRanges > 0 ? new D3D12_DESCRIPTOR_RANGE1[NumDescriptorRanges] : nullptr;
				
				memcpy(pDescirptorRanges, RootParameter.DescriptorTable.pDescriptorRanges,
					sizeof(D3D12_DESCRIPTOR_RANGE1) * NumDescriptorRanges);
				
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
		VersionRootSignatureDesc.Init_1_1(NumParameters, pParameters, NumStaticSamplers, pStaticSamplers, Flags);
		
	}
}
