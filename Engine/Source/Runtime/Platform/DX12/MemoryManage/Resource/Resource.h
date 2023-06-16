#pragma once
#include "../../Common/DX12Header.h"
#include "Core.h"

namespace Zero
{
	class FBuddyAllocator;
	class FDX12Resource;

	struct FBuddyBlockData
	{
		uint32_t Offset = 0;
		uint32_t Order = 0;
		uint32_t ActualUsedSize = 0;

		Ref<FDX12Resource> PlacedResource = nullptr;
	};

	class FResourceLocation
	{
	public:
		enum class EResourceLocationType
		{
			Undefined,
			StandAlone,
			SubAllocation,
		};
		FResourceLocation();
		~FResourceLocation();

		void ReleaseResource();

		void SetType(EResourceLocationType Type) { m_ResourceLocationType = Type; }

		Ref<FDX12Resource> GetResource() { return m_UnderlyingResource; }
		void* GetMapAddress() {return m_MappedAddress;}
	public:
		EResourceLocationType m_ResourceLocationType = EResourceLocationType::Undefined;

		// SubAllocation 
		FBuddyAllocator* m_Allocator = nullptr;

		FBuddyBlockData m_BlockData;

		// StandAlone resource 
		Ref<FDX12Resource> m_UnderlyingResource = nullptr;

		union
		{
			uint64_t m_OffsetFromBaseOfResource;
			uint64_t m_OffsetFromBaseOfHeap;
		};


		D3D12_GPU_VIRTUAL_ADDRESS m_GPUVirtualAddress = 0;

		// About mapping, for upload buffer
		void* m_MappedAddress = nullptr;
	};

	class FDX12Device;

	class FDX12Resource : public std::enable_shared_from_this<FDX12Resource>
	{
		friend class FBuddyAllocator;
	public:
		FDX12Resource();
		FDX12Resource(const std::string& ResourceName, const D3D12_RESOURCE_DESC& ResourceDesc,
			const D3D12_CLEAR_VALUE* FTextureClearValue = nullptr);
		FDX12Resource(const std::string& ResourceName, ComPtr<ID3D12Resource> Resource,
			const D3D12_CLEAR_VALUE* FTextureClearValue = nullptr);

		void SetName(const std::wstring& Name);
		const std::wstring& GetName() const { return m_ResourceName; }
		virtual void SetResource(ComPtr<ID3D12Resource> Resource);

		void SetGpuVirtualAddress();


		/**
		* Check if the resource format supports a specific feature.
		*/
		virtual bool CheckFormatSupport(D3D12_FORMAT_SUPPORT1 InFormatSupport) const { return (m_FormatSupport.Support1 & InFormatSupport) != 0; }
		virtual bool CheckFormatSupport(D3D12_FORMAT_SUPPORT2 InFormatSupport) const { return (m_FormatSupport.Support2 & InFormatSupport) != 0; }

		ComPtr<ID3D12Resource> GetD3DResource() const { return m_D3DResource; }

		D3D12_CLEAR_VALUE* GetClearValuePtr() { return m_D3DClearValue.get(); }

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

		void Map();
		virtual ~FDX12Resource();
	protected:
		std::wstring                 m_ResourceName;
		ComPtr<ID3D12Resource>   	m_D3DResource;
		D3D12_FEATURE_DATA_FORMAT_SUPPORT  m_FormatSupport;
		Scope<D3D12_CLEAR_VALUE>     m_D3DClearValue;

		D3D12_GPU_VIRTUAL_ADDRESS m_GPUVirtualAddress = 0;
		// For upload buffer
		void* m_MappedBaseAddress = nullptr;

	};

	template<typename T>
	class FResourceScopeMap
	{
	public:
		FResourceScopeMap(FDX12Resource* Resource)
		{
			m_D3DResource = Resource->GetD3DResource().Get();
			m_D3DResource->Map(0, nullptr, reinterpret_cast<void**>(&MappedData));
		}

		~FResourceScopeMap()
		{
			m_D3DResource->Unmap(0, nullptr);
		}

		T* GetMappedData() { return MappedData; }

	private:
		ID3D12Resource* m_D3DResource = nullptr;

		T* m_MappedData = nullptr;
	};
}