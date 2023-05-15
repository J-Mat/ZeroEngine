#pragma once

#include "Core.h"
#include "Common/DX12Header.h"
#include "Render/RHI/CommandList.h"
#include "MemoryManage/Resource/UploadBuffer.h"
#include "MemoryManage/Resource/ResourceStateTracker.h"
#include "Render/Moudule/Texture/Image.h"
#include "DX12Device.h"
#include "MemoryManage/Descriptor/DynamicDescriptorHeap.h"
#include "MemoryManage/Descriptor/DescriptorCache.h"
#include "./PSO/GenerateMipsPSO.h"


namespace Zero
{
	class FDX12PipelineStateObject;
	class FDX12Resource;
	class FDX12Texture2D;


	class FDX12CommandList : public FCommandList, public std::enable_shared_from_this<FDX12CommandList>
	{
		struct FResourceTransitionRecord
		{
			ID3D12Resource* Resource{};
			D3D12_RESOURCE_STATES StateAfter{};
			void Reset() { Resource = nullptr; }
			bool IsNull() { return Resource == nullptr; }
		};
	public:
		FDX12CommandList(D3D12_COMMAND_LIST_TYPE Type);
		virtual ~FDX12CommandList() = default;

		void Init();
		void FlushResourceBarriers();
		void OnDeployed();
		ComPtr<ID3D12GraphicsCommandList> GetD3D12CommandList() { return m_D3DCommandList; }

		void CopyBufferRegion(ComPtr<ID3D12Resource> DstResource, UINT64 DstOffset, ComPtr<ID3D12Resource> SrcResource, UINT64 SrcOffset, UINT64 Size);
		void CopyTextureRegion(const D3D12_TEXTURE_COPY_LOCATION* Dst, UINT DstX, UINT DstY, UINT DstZ, const D3D12_TEXTURE_COPY_LOCATION* Src, const D3D12_BOX* SrcBox);
		ComPtr<ID3D12Resource> CreateDefaultBuffer(const void* BufferData, size_t BufferSize, D3D12_RESOURCE_FLAGS Flags = D3D12_RESOURCE_FLAG_NONE);
		void CreateAndInitDefaultBuffer(const void* BufferData, uint32_t Size, uint32_t Alignment, FResourceLocation& ResourceLocation);
		void CreateAndInitDefaultBuffer(const void* BufferData, uint32_t Size, ComPtr<ID3D12Resource> Resource);
	
		Ref<FDX12Resource> CreateTextureResource(const std::string& TextureName, Ref<FImage> Image, bool bGenerateMip = false);
		void AllocateTextureResource(const std::string& TextureName, const FTextureDesc& TextureDesc, FResourceLocation& ResourceLocation, Ref<FImage> Image = nullptr, bool bGenerateMip = false);
		void AllocateTextureCubemapResource(const std::string& TextureName, const FTextureDesc& TextureDesc, FResourceLocation& ResourceLocation,  Ref<FImage> ImageData[CUBEMAP_TEXTURE_CNT]);
		void GenerateMip(Ref<FDX12Resource> TextureResource);
		void GenerateMipSimple(Ref<FDX12Resource> TextureResource);
		void GenerateMips_UAV(Ref<FDX12Texture2D> Texture, bool bIsSRGB);
		ComPtr<ID3D12Resource> CreateTextureCubemapResource(Ref<FImage> ImageData[CUBEMAP_TEXTURE_CNT], uint64_t Width, uint32_t Height, uint32_t Chanels);
		ComPtr<ID3D12Resource> CreateRenderTargetResource(uint32_t Width, uint32_t Height);

		void ResolveSubResource(const Ref<FDX12Resource>& DstRes, const Ref<FDX12Resource> SrcRes, uint32_t DstSubRes = 0, uint32_t SrcSubRes = 0);

		void ClearTexture(FDX12Texture2D* TexturePtr, const ZMath::vec4 Color);
		void ClearDepthStencilTexture(FDX12Texture2D* TexturePtr, D3D12_CLEAR_FLAGS ClearFlags, float Depth = 1.0f, uint8_t Stencil = 0);

		/**
		* Copy resources.
		*/
		void CopyResource(const Ref<FDX12Resource>& DstRes, const Ref<FDX12Resource>& SrcRes);
		void CopyResource(ComPtr<ID3D12Resource> DstRes, ComPtr<ID3D12Resource> SrcRes);
		void CopyResource(ID3D12Resource* DstRes, ID3D12Resource* SrcRes);
		virtual void CopyResource(void* Dst, void* Src);

		/**
		* Draw geometry.
		*/
		void Draw(uint32_t vertexCount, uint32_t instanceCount = 1, uint32_t startVertex = 0, uint32_t startInstance = 0);
		void DrawIndexed(uint32_t indexCount, uint32_t instanceCount = 1, uint32_t startIndex = 0, int32_t baseVertex = 0,
			uint32_t startInstance = 0);

		void Dispatch(uint32_t NumGroupsX, uint32_t NumGroupsY, uint32_t NumGroupsZ);
		/**
		 * Close the command list.
		* Used by the command queue.
		*
		* @param pendingCommandList The command list that is used to execute pending
		* resource barriers (if any) for this command list.
		*
		* @return true if there are any pending resource barriers that need to be
		* processed.
		 */
		bool Close(const Ref<FDX12CommandList>& PendingCommandList);
		// Just close the command list. This is useful for pending command lists.
		void Close();

		/**
		* Release tracked objects. Useful if the swap chain needs to be resized.
		*/
		void ReleaseTrackedObjects();


		// Binds the current descriptor heaps to the command list.
		void BindDescriptorHeaps();

		/**
		* Set the pipeline state object on the command list.
		*/
		void SetPipelineState(const Ref<FDX12PipelineStateObject>& PipelineState);
		void SetPipelineState(ComPtr<ID3D12PipelineState> D3DPipelineState);

		/**
		* Transition a resource to a particular state.
		*
		* @param resource The resource to transition.
		* @param stateAfter The state to transition the resource to. The before state is resolved by the resource state
		* tracker.
		* @param subresource The subresource to transition. By default, this is D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES
		* which indicates that all subresources are transitioned to the same state.
		* @param bFlushBarriers Force flush any barriers. Resource barriers need to be flushed before a command (draw,
		* dispatch, or copy) that expects the resource to be in a particular state can run.
		*/
		void TransitionBarrier(const Ref<FDX12Resource>& Resource, D3D12_RESOURCE_STATES StateAfter,
			UINT Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, bool bFlushBarriers = false);
		void TransitionBarrier(ID3D12Resource* Resource, D3D12_RESOURCE_STATES StateAfter,
			UINT Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, bool bFlushBarriers = false);
		void TransitionBarrier(ComPtr<ID3D12Resource> Resource, D3D12_RESOURCE_STATES StateAfter,
			UINT Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, bool bFlushBarriers = false);
		virtual void TransitionBarrier(void* Resource, EResourceState ResourceState, UINT Subresource = -1, bool bFlushBarriers = false) override;

		/**
		 * Add a UAV barrier to ensure that any writes to a resource have completed
		 * before reading from the resource.
		 *
		 * @param [resource} The resource to add a UAV barrier for (can be null).
		 * @param flushBarriers Force flush any barriers. Resource barriers need to be
		 * flushed before a command (draw, dispatch, or copy) that expects the resource
		 * to be in a particular state can run.
		 */
		void UAVBarrier(const Ref<FDX12Resource>& Resource = nullptr, bool bFlushBarriers = false);
		void UAVBarrier(ComPtr<ID3D12Resource> Resource, bool bFlushBarriers = false);


		/**
		 * Add an aliasing barrier to indicate a transition between usages of two
		 * different resources that occupy the same space in a heap.
		 *
		 * @param [beforeResource] The resource that currently occupies the heap (can be null).
		 * @param [afterResource] The resource that will occupy the space in the heap (can be null).
		 */
		void AliasingBarrier(const Ref<FDX12Resource>& BeforeResource  = nullptr, 
							 const Ref<FDX12Resource>& AfterResource = nullptr, 
							 bool bFlushBarriers = false);
		void AliasingBarrier(ComPtr<ID3D12Resource> BeforeResource,
							 ComPtr<ID3D12Resource> AfterResource, 
			                 bool bFlushBarriers = false);


		Ref<FDX12CommandList> GetGenerateMipsCommandList() const
		{
			return m_ComputeCommandList;
		}

		void SetGraphicsRootSignature(Ref<FDX12RootSignature> RootSignature);
		void SetComputeRootSignature(Ref<FDX12RootSignature> RootSignature);

		virtual void Reset();
		virtual void Execute();

		/**
		* Set a set of 32-bit constants on the compute pipeline.
		*/
		void SetCompute32BitConstants(uint32_t RootParameterIndex, uint32_t NumConstants, const void* Constants);
		template<typename T>
		void SetCompute32BitConstants(uint32_t RootParameterIndex, const T& Constants)
		{
			CORE_ASSERT(sizeof(T) % sizeof(uint32_t) == 0, "Size of type must be a multiple of 4 bytes");
			SetCompute32BitConstants(RootParameterIndex, sizeof(T) / sizeof(uint32_t), &Constants);
		}
		
		/**
		* Set the SRV on the graphics pipeline.
		*/
		void SetShaderResourceView(uint32_t RootParameterIndex, uint32_t DescriptorOffset, const Ref<FDX12ShaderResourceView>& SRV,
			D3D12_RESOURCE_STATES StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
			UINT FirstSubresource = 0,
			UINT NumSubresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);


		/**
		* Set an inline UAV.
		*
		* Note: Only Buffer resoruces can be used with inline UAV's.
		*/
		void SetUnorderedAccessView(uint32_t RootParameterIndex, uint32_t DescriptorOffset,  const Ref<FDX12UnorderedAccessResourceView>& Uav,
			D3D12_RESOURCE_STATES StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
			UINT FirstSubresource = 0,
			UINT NumSubresources = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);

		CD3DX12_GPU_DESCRIPTOR_HANDLE AppendCbvSrvUavDescriptors(D3D12_CPU_DESCRIPTOR_HANDLE* DstDescriptor, uint32_t NumDescriptors);


	private:
		void TrackResource(Microsoft::WRL::ComPtr<ID3D12Object> Object);
		void TrackResource(const Ref<FDX12Resource>& res);

		/**
		* Set the currently bound descriptor heap.
		* Should only be called by the DynamicDescriptorHeap class.
		*/
		void SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE HeapType, ID3D12DescriptorHeap* Heap);

	private:
		FResourceTransitionRecord m_ResourceTransitionRecord;

		Scope<FDescriptorCache> m_DescriptorCache;
		Scope<FGenerateMipsPSO> m_GenerateMipsPSO = nullptr;
		std::vector<ComPtr<ID3D12Object>> m_TrackedObjects;

		D3D12_COMMAND_LIST_TYPE  m_CommandListType;
		ComPtr<ID3D12CommandAllocator>	m_CommandAllocator = nullptr;
		ComPtr<ID3D12GraphicsCommandList> m_D3DCommandList = nullptr;
		Scope<FUploadBuffer> m_UploadBuffer;
		Scope<FResourceStateTracker> m_ResourceStateTracker = nullptr;
		ID3D12DescriptorHeap* m_DescriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];

		// The dynamic descriptor heap allows for descriptors to be staged before
		// being committed to the command list. Dynamic descriptors need to be
		// committed before a Draw or Dispatch.
		Scope<FDynamicDescriptorHeap> m_DynamicDescriptorHeap[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];


		// For copy queues, it may be necessary to generate mips while loading textures.
		// Mips can't be generated on copy queues but must be generated on compute or
		// direct queues. In this case, a Compute command list is generated and executed
		// after the copy queue is finished uploading the first sub resource.
		Ref<FDX12CommandList> m_ComputeCommandList = nullptr;


		// Keep track of the currently bound root signatures to minimize root
		// signature changes.
		ID3D12RootSignature* m_RootSignature = nullptr;
		// Keep track of the currently bond pipeline state object to minimize PSO changes.
		ID3D12PipelineState* m_PipelineState = nullptr;
	};
}