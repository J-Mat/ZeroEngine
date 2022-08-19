#include "../Common/DX12Header.h"
#include "Core.h"

namespace Zero
{
	class IResource
	{
	public:
		IResource(FDX12Device& Device);
		IResource(FDX12Device& Device, const D3D12_RESOURCE_DESC& ResourceDesc,
			const D3D12_CLEAR_VALUE* ClearValue = nullptr);
		IResource(FDX12Device& Device, ComPtr<ID3D12Resource> Resource,
			const D3D12_CLEAR_VALUE* ClearValue = nullptr);

		void SetName(const std::wstring& Name) { m_ResourceName = Name; };
		const std::wstring& GetName() const { return m_ResourceName; }
		virtual void SetResource(ComPtr<ID3D12Resource> Resource);


		/**
		* Check if the resource format supports a specific feature.
		*/
		bool CheckFormatSupport(D3D12_FORMAT_SUPPORT1 InFormatSupport) const { return (m_FormatSupport.Support1 & InFormatSupport) != 0; }
		bool CheckFormatSupport(D3D12_FORMAT_SUPPORT2 InFormatSupport) const { return (m_FormatSupport.Support2 & InFormatSupport) != 0; }

		ComPtr<ID3D12Resource> GetD3DResource() { return m_D3DResource; }

		D3D12_RESOURCE_DESC GetD3D12ResourceDesc() const
		{
			D3D12_RESOURCE_DESC ResDesc = {};
			if (m_D3DResource)
			{
				ResDesc = m_D3DResource->GetDesc();
			}

			return ResDesc;
		}

	protected:

		virtual ~IResource() = default;


		FDX12Device& m_Device;
		std::wstring                 m_ResourceName;
		ComPtr<ID3D12Resource>   	m_D3DResource;
		D3D12_FEATURE_DATA_FORMAT_SUPPORT  m_FormatSupport;
		Scope<D3D12_CLEAR_VALUE>     m_D3DClearValue;
	private:
		void CheckFeatureSupport();
	};
}