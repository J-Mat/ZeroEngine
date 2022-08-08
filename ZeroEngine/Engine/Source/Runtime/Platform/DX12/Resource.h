#include "Common/DX12Header.h"
#include "Core.h"
#include "DX12Device.h"

namespace Zero
{
	class FResource
	{
	public:
		void SetName( const std::string& name );
    	const std::string& GetName() const { return ResourceName;}
	

	  	/**
     	* Check if the resource format supports a specific feature.
     	*/
    	bool CheckFormatSupport( D3D12_FORMAT_SUPPORT1 InFormatSupport ) const {return (InFormatSupport.Support1 &  FormatSupport) != 0;};
    	bool CheckFormatSupport( D3D12_FORMAT_SUPPORT1 InFormatSupport ) const {return (InFormatSupport.Support2 &  FormatSupport) != 0;};
	protected:
		FResource(FDX12Device &InDevice, const D3D12_RESOURCE_DESC& ResourceDesc,
				const D3D12_CLEAR_VALUE* ClearValue = nullptr);
		FResource(FDX12Device &InDevice, ComPtr<ID3D12Resource> Resource,
				const D3D12_CLEAR_VALUE* ClearValue = nullptr)

		virtual ~Resource() = default;

		std::string                 ResourceName;
		ComPtr<ID3D12Resource>   	D3DResource;
		D3D12_FEATURE_DATA_FORMAT_SUPPORT  FormatSupport;
		Scope<D3D12_CLEAR_VALUE>     D3DClearValue;
	private:
		void CheckFeatureSupport();
	}
}