#pragma once

#include "Core.h"
#include "Common/DX12Header.h"
#include "Render/RHI/GraphicDevice.h"
#include "./Common/d3dx12.h"
#include <wrl.h>
#include "DX12CommandQueue.h"


namespace Zero
{
	class FDX12Device : public IDevice
	{
	public:
		virtual void Init();

		ID3D12Device* GetDevice() { return m_D3DDevice.Get(); }
		FDX12CommandQueue& GetCommandQueue(D3D12_COMMAND_LIST_TYPE Type = D3D12_COMMAND_LIST_TYPE_DIRECT);

		D3D_ROOT_SIGNATURE_VERSION GetHighestRootSignatureVersion() const
		{
			return m_HighestRootSignatureVersion;
		}
	private:
		void EnableDebugLayer();
		void CreateDevice();
		void GetDescriptorSize();
		void CreateCommandQueue();
		void CheckFeatures();

		
	private:
		UINT m_RtvDescriptorSize;
		UINT m_DsvDescriptorSize;
		UINT m_Cbv_Srv_UavDescriptorSize;
		ComPtr<IDXGIFactory4> m_DxgiFactory;
		ComPtr<ID3D12Device> m_D3DDevice;

		Scope<FDX12CommandQueue> m_DirectCommandQueue;
		Scope<FDX12CommandQueue> m_ComputeCommandQueue;
		Scope<FDX12CommandQueue> m_CopyCommandQueue;


		D3D_ROOT_SIGNATURE_VERSION m_HighestRootSignatureVersion;
	};
}
