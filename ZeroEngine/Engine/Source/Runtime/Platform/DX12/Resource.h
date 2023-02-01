#pragma once
#include "../Common/DX12Header.h"
#include "Core.h"

namespace Zero
{
	class FDX12Device;
	class FResource 
	{
	public:
		FResource();
		FResource(const std::string& ResourceName, const D3D12_RESOURCE_DESC& ResourceDesc,
			const D3D12_CLEAR_VALUE* FTextureClearValue = nullptr);
		FResource(ComPtr<ID3D12Resource> Resource,
			const D3D12_CLEAR_VALUE* FTextureClearValue = nullptr);

		void SetName(const std::wstring& Name);
		const std::wstring& GetName() const { return m_ResourceName; }
		virtual void SetResource(ComPtr<ID3D12Resource> Resource);


		/**
		* Check if the resource format supports a specific feature.
		*/
		virtual bool CheckFormatSupport(D3D12_FORMAT_SUPPORT1 InFormatSupport) const { return (m_FormatSupport.Support1 & InFormatSupport) != 0; }
		virtual bool CheckFormatSupport(D3D12_FORMAT_SUPPORT2 InFormatSupport) const { return (m_FormatSupport.Support2 & InFormatSupport) != 0; }

		ComPtr<ID3D12Resource> GetD3DResource() const { return m_D3DResource; }

		D3D12_RESOURCE_DESC GetD3D12ResourceDesc() const
		{
			D3D12_RESOURCE_DESC ResDesc = {};
			if (m_D3DResource)
			{
				ResDesc = m_D3DResource->GetDesc();
			}

			return ResDesc;
		}

        bool CheckSRVSupport() const
        {
            return CheckFormatSupport(D3D12_FORMAT_SUPPORT1_SHADER_SAMPLE);
        }

        bool CheckRTVSupport() const
        {
            return CheckFormatSupport(D3D12_FORMAT_SUPPORT1_RENDER_TARGET);
        }

        bool CheckUAVSupport() const
        {
            return CheckFormatSupport(D3D12_FORMAT_SUPPORT1_TYPED_UNORDERED_ACCESS_VIEW) &&
                CheckFormatSupport(D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD) &&
                CheckFormatSupport(D3D12_FORMAT_SUPPORT2_UAV_TYPED_STORE);
        }

        bool CheckDSVSupport() const
        {
            return CheckFormatSupport(D3D12_FORMAT_SUPPORT1_DEPTH_STENCIL);
        }

		void CheckFeatureSupport();
		virtual ~FResource();
	protected:
		std::wstring                 m_ResourceName;
		ComPtr<ID3D12Resource>   	m_D3DResource;
		D3D12_FEATURE_DATA_FORMAT_SUPPORT  m_FormatSupport;
		Scope<D3D12_CLEAR_VALUE>     m_D3DClearValue;
	};
}