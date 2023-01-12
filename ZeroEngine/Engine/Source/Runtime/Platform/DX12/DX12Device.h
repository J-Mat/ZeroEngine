#pragma once

#include "Core.h"
#include "Render/RHI/GraphicDevice.h"
#include "Common/DX12Header.h"
#include <wrl.h>
#include "Adapter.h"
#include "MemoryManage/DescriptorAllocation.h"
#include "DX12SwapChain.h"


namespace Zero
{
	//class FDX12SwapChain;
	class FDX12CommandQueue;
	class FDescriptorAllocator;
	class FDX12Device : public IDevice, public std::enable_shared_from_this<FDX12Device>
	{
	public:
		FDX12Device();
		virtual void Init();
		static FDX12Device* Get() { return m_Instance; }
		
		Ref<FDX12Device> AsShared() { return shared_from_this(); }

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
		FDescriptorAllocation AllocateRuntimeDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE Type, uint32_t NumDescriptors = 1);
		
		void CreateGuiDescHeap(uint32_t NumDescriptors = 256);
		ComPtr<ID3D12DescriptorHeap> GetGuiDescHeap() { return m_GUISrvDescHeap; }
		FLightDescrptorAllocation AllocateGuiDescritor();
		/**
		* Release stale descriptors. This should only be called with a completed frame counter.
		*/
		void ReleaseStaleDescriptors();

		/**
		* Get the adapter that was used to create this device.
		*/
		std::shared_ptr<FAdapter> GetAdapter() const { return m_Adapter; }
		
		virtual void Flush() override;
		
		virtual void CreateSwapChain(HWND hWnd) override;
		virtual Ref<FSwapChain>  GetSwapChain() { return m_SwapChain; }

		virtual void Resize(uint32_t Width, uint32_t Height) { m_SwapChain->Resize(Width, Height); }
		
		static std::vector<CD3DX12_STATIC_SAMPLER_DESC> GetStaticSamplers();

		virtual void PreInitWorld() override;

		virtual void FlushInitCommandList() override;
		
		uint32_t RegisterActiveComandlist(Ref<FDX12CommandList> CommandList);
		void UnRegisterActiveComandlist(uint32_t ID);
		Ref<FDX12CommandList> GetActiveCommandList(uint32_t Slot);
		Ref<FDX12CommandList> GetRenderCommandList() { return m_RenderCommandList; }
		void SetRenderCommandList(Ref<FDX12CommandList> CommandList) { m_RenderCommandList = CommandList; }
		Ref<FDX12CommandList> GetInitWorldCommandList() { return m_InitWorldCommandList; }
		void SetInitWorldCommandList(Ref<FDX12CommandList> CommandList) { m_InitWorldCommandList = CommandList; }

	private:
		static FDX12Device* m_Instance;
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
		
		ComPtr<ID3D12DescriptorHeap> m_GUISrvDescHeap = nullptr;
		int32_t m_CurGuiDescHeapIndex = 0;

		D3D_ROOT_SIGNATURE_VERSION m_HighestRootSignatureVersion;
		
		Ref<FDX12SwapChain> m_SwapChain;

		ComPtr<ID3D12DescriptorHeap> m_D3DGuiDescHeap = nullptr;
		
		uint32_t m_CommandListID = 0;
		std::map<uint32_t, Ref<FDX12CommandList>> m_ActiveCommandListMap;
		Ref<FDX12CommandList> m_RenderCommandList = nullptr;;
		Ref<FDX12CommandList> m_InitWorldCommandList = nullptr;;
	};
}
