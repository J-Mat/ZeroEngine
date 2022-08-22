#pragma once

#include "Core.h"
#include "Render/RHI/GraphicDevice.h"
#include "Common/DX12Header.h"
#include <wrl.h>
#include "Adapter.h"
#include "GPUMemory/DescriptorAllocation.h"


namespace Zero
{
	class FDX12CommandQueue;
	class FDX12SwapChain;
	class FDescriptorAllocator;
	class FDX12Device : public IDevice
	{
	public:
		FDX12Device() = default;
		virtual void Init();

		ID3D12Device* GetDevice() { return m_D3DDevice.Get(); }
		FDX12CommandQueue& GetCommandQueue(D3D12_COMMAND_LIST_TYPE Type = D3D12_COMMAND_LIST_TYPE_DIRECT);

		D3D_ROOT_SIGNATURE_VERSION GetHighestRootSignatureVersion() const
		{
			return m_HighestRootSignatureVersion;
		}

		/**
		* Get a description of the adapter that was used to create the device.
		*/
		std::wstring GetDescription() const;

		/**
		 * Gets the size of the handle increment for the given type of descriptor heap.
		 */
		UINT GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE Type) const
		{
			return m_D3DDevice->GetDescriptorHandleIncrementSize(Type);
		}

		/**
		* Allocate a number of CPU visible descriptors.
		*/
		FDescriptorAllocation AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE Type, uint32_t NumDescriptors = 1);

		/**
		* Release stale descriptors. This should only be called with a completed frame counter.
		*/
		void ReleaseStaleDescriptors();

		/**
		* Get the adapter that was used to create this device.
		*/
		std::shared_ptr<FAdapter> GetAdapter() const { return m_Adapter; }
		
		void Flush();
		
		void CreatSwapChain(HWND hWnd);
		Ref<FDX12SwapChain> GetSwapChain() { return m_SwapChain; }
		

	private:
		void EnableDebugLayer();
		void CreateDevice();
		void GetDescriptorSize();
		void CreateCommandQueue();
		void CreateDescriptors();
		void CheckFeatures();

		
	private:
		UINT m_RtvDescriptorSize;
		UINT m_DsvDescriptorSize;
		UINT m_Cbv_Srv_UavDescriptorSize;
		ComPtr<ID3D12Device> m_D3DDevice;
		Ref<FAdapter> m_Adapter;

		Scope<FDX12CommandQueue> m_DirectCommandQueue;
		Scope<FDX12CommandQueue> m_ComputeCommandQueue;
		Scope<FDX12CommandQueue> m_CopyCommandQueue;

		Ref<FDescriptorAllocator> m_DescriptorAllocators[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];

		D3D_ROOT_SIGNATURE_VERSION m_HighestRootSignatureVersion;
		
		Ref<FDX12SwapChain> m_SwapChain;
	};
}
