#pragma once

#include "Core.h"
#include "Render/RHI/GraphicDevice.h"
#include "Common/DX12Header.h"
#include <wrl.h>
#include "Adapter.h"
#include "MemoryManage/Descriptor/DescriptorAllocation.h"
#include "DX12SwapChain.h"
#include "Render/RHI/GraphicPipelineStateObject.h"
#include "MemoryManage/Resource/ResourceAllocator.h"
#include "MemoryManage/ConstantBuffer/LinearDynamicAllocator.h"
#include "D3D12MemAlloc/D3D12MemAlloc.h"


namespace Zero
{
	//class FDX12SwapChain;
	class FDX12CommandQueue;
	class FDescriptorAllocator;
	class FDX12Texture2D;
	struct FGraphicPSODescriptor;
	class FDX12Device : public IDevice, public std::enable_shared_from_this<FDX12Device>
	{
	public:
		FDX12Device();
		virtual void Init();
		static FDX12Device* Get() { return m_Instance; }
		
		Ref<FDX12Device> AsShared() { return shared_from_this(); }

		ID3D12Device* GetDevice() { return m_D3DDevice.Get(); }
		FDX12CommandQueue& GetCommandQueue(ERenderPassType Type);

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
		
		static std::vector<CD3DX12_STATIC_SAMPLER_DESC> GetStaticSamplers(std::vector<EShaderSampler>& ShaderSamplers);


		virtual FCommandListHandle GenerateCommanList(ERenderPassType RenderPassType = ERenderPassType::Graphics) override;
		Ref<FDX12CommandList> GetCommandList(FCommandListHandle Handle);
		virtual Ref<FCommandList> GetRHICommandList(FCommandListHandle Handle) override;
		virtual FCommandListHandle GetSingleThreadCommadList(ERenderPassType RenderPassType = ERenderPassType::Graphics) override { return m_SingleThreadCommandListHandles[RenderPassType]; }

		void SetInitWorldCommandList(FCommandListHandle Handle) {  m_InitWorldCommandListHandle = Handle; }
		void SetMipCommandList(FCommandListHandle Handle) {  m_MipCommandListHandle = Handle; }
		FCommandListHandle GetInitWorldCommadListHandle() { return m_InitWorldCommandListHandle; };
		Ref<FDX12CommandList> GetInitWorldCommandList() { return GetCommandList(m_InitWorldCommandListHandle); }
		Ref<FDX12CommandList> GetMipCommandList() { return GetCommandList(m_MipCommandListHandle); }
		virtual void PreInitWorld() override;
		virtual void FlushInitCommandList() override;
		
		uint64_t GetCurrentFenceValue();
		void ClearBackBuffer();

		virtual void BeginFrame() override;
		void InitSingleThreadCommandLists();
		void ExecuteSingleThreadCommandLists();
		virtual void EndFrame() override;
		
		FDX12Texture2D* CreateDepthTexture(const std::string& TextureName, uint32_t Width, uint32_t Height);


		uint32_t RegisterActiveComandlist(Ref<FDX12CommandList> CommandList);
		void UnRegisterActiveComandlist(uint32_t ID);
		Ref<FDX12CommandList> GetActiveCommandList(uint32_t Slot);
	
		FUploadBufferAllocator* GetUploadBufferAllocator() { return m_UploadBufferAllocator.get(); }

		FReadBackBufferAllocator* GetReadBackBufferAllocator() { return m_ReadBackBufferAllocator.get(); }

		FDefaultBufferAllocator* GetDefaultBufferAllocator() { return m_DefaultBufferAllocator.get(); }

		FTextureResourceAllocator* GetTextureResourceAllocator() { return m_TextureResourceAllocator.get(); }

		FLinearDynamicAllocator* GetLinearDynamicAllocator();

		D3D12MA::Allocator* GetMemAllocator() const;

	public:
		virtual Ref<FGraphicPipelineStateObject> CreateGraphicPSO(const FGraphicPSODescriptor& PSODescriptor) override;
		virtual Ref<FComputePipelineStateObject> CreateComputePSO(const FComputePSODescriptor& PSODescriptor) override;
		virtual FTexture2D* CreateTexture2D(const std::string& TextureName, const FTextureDesc& Desc, bool bCreateTextureView = true) override;
		virtual FTextureCube* CreateTextureCube(const std::string& TextureName, const FTextureDesc& Desc, bool bCreateTextureView = true) override;
		virtual Ref<FTexture2D> GetOrCreateTexture2D(const std::string& Filename, bool bNeedMipMap = false) override;
		virtual Ref<FTextureCube> GetOrCreateTextureCubemap(FTextureHandle Handles[CUBEMAP_TEXTURE_CNT], std::string TextureCubemapName) override;
		virtual Ref<FRenderTarget2D> CreateRenderTarget2D(const FRenderTarget2DDesc& Desc) override;
		virtual FRenderTarget2D* CreateRenderTarget2D() override;
		virtual FRenderTargetCube* CreateRenderTargetCube() override;
		virtual Ref<FRenderTargetCube> CreateRenderTargetCube(const FRenderTargetCubeDesc& Desc) override;
		virtual Ref<FShader> CreateGraphicShader(const FShaderDesc& ShaderDesc);
		virtual Ref<FShader> CreateComputeShader(const FShaderDesc& ShaderDesc);
		virtual Ref<FMesh> CreateMesh(const std::vector<FMeshData>& MeshDatas, FVertexBufferLayout& Layout);
		virtual Ref<FMesh> CreateMesh(float* Vertices, uint32_t VertexCount, uint32_t* Indices, uint32_t IndexCount, FVertexBufferLayout& Layout);
		virtual FBuffer* CreateBuffer(const FBufferDesc& Desc) override;
		virtual void BindVertexBuffer(FCommandListHandle Handle, FBuffer* VertexBuffer) override;
		virtual void BindIndexBuffer(FCommandListHandle Handle, FBuffer* IndexBuffer) override;
	private:
		static FDX12Device* m_Instance;
		void EnableDebugLayer();
		void CreateDevice();
		void GetDescriptorSize();
		void CreateCommandQueue();
		void CreateDescriptors();
		void CheckFeatures();

		
	private:
		FCommandListHandle m_SingleThreadCommandListHandles[3] = { {}, {}, {} };
		FCommandListHandle m_InitWorldCommandListHandle{};
		FCommandListHandle m_MipCommandListHandle{};
		UINT m_RtvDescriptorSize;
		UINT m_DsvDescriptorSize;
		UINT m_Cbv_Srv_UavDescriptorSize;
		ComPtr<ID3D12Device> m_D3DDevice;
		Ref<FAdapter> m_Adapter;

		std::vector<Scope<FDX12CommandQueue>> m_CommandQueue;
		std::vector<std::vector<Ref<FDX12CommandList>>> m_CommandLists;


		Scope<FUploadBufferAllocator> m_UploadBufferAllocator;

		Scope<FReadBackBufferAllocator> m_ReadBackBufferAllocator;

		Scope<FDefaultBufferAllocator> m_DefaultBufferAllocator;

		Scope<FTextureResourceAllocator> m_TextureResourceAllocator;

		std::vector<Scope<FLinearDynamicAllocator>> m_DynamicAllocators;

		ReleasablePtr<D3D12MA::Allocator> m_MemAllocator = nullptr;

		Ref<FDescriptorAllocator> m_DescriptorAllocators[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
		
		ComPtr<ID3D12DescriptorHeap> m_GUISrvDescHeap = nullptr;
		int32_t m_CurGuiDescHeapIndex = 0;

		D3D_ROOT_SIGNATURE_VERSION m_HighestRootSignatureVersion;
		
		Ref<FDX12SwapChain> m_SwapChain;

		ComPtr<ID3D12DescriptorHeap> m_D3DGuiDescHeap = nullptr;
		
		uint32_t m_CommandListID = 0;
		std::map<uint32_t, Ref<FDX12CommandList>> m_ActiveCommandListMap;
	};
}
