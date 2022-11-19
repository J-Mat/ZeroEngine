#pragma once

#include "Core.h"
#include "Common/DX12Header.h"
#include <dxgi1_6.h>
#include <wrl/client.h>


namespace Zero
{
	class FAdapter;
	using FAdapterList = std::vector<Ref<FAdapter>>;
	class FAdapter
	{
	public:
		/**
		* Create a GPU adapter.
		*
		* @param gpuPreference The GPU preference. By default, a high-performance
		* GPU is preferred.
		* @param useWarp If true, create a WARP adapter.
		*
		* @returns A shared pointer to a GPU adapter or nullptr if the adapter
		* could not be created.
		*/
		static Ref<FAdapter> Create(DXGI_GPU_PREFERENCE GpuPreference = DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, bool bUseWarp = false);
		static FAdapterList GetAdapters(DXGI_GPU_PREFERENCE GpuPreference = DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE);

		FAdapter(ComPtr<IDXGIAdapter4> DxgiAdapter);
		virtual ~FAdapter() = default;

		ComPtr<IDXGIAdapter4> GetDXGIAdapter() const { return m_DxgiAdapter; }

		const std::wstring GetDescription() const { return m_Desc.Description; }


	private:
		ComPtr<IDXGIAdapter4> m_DxgiAdapter;
		DXGI_ADAPTER_DESC3 m_Desc;
	};
}