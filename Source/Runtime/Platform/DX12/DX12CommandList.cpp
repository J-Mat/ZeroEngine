#include "DX12CommandList.h"
#include "DX12Texture2D.h"
#include "PipelineStateObject.h"
#include "DX12RenderTarget.h"
#include "DX12RootSignature.h"

namespace Zero
{
	FDX12CommandList::FDX12CommandList(D3D12_COMMAND_LIST_TYPE Type)
	: m_CommandListType(Type)
	, m_RootSignature(nullptr)
	{
		ThrowIfFailed(FDX12Device::Get()->GetDevice()->CreateCommandAllocator(m_CommandListType, IID_PPV_ARGS(&m_CommandAllocator)));

		ThrowIfFailed(FDX12Device::Get()->GetDevice()->CreateCommandList(0, m_CommandListType, m_CommandAllocator.Get(),
			nullptr, IID_PPV_ARGS(&m_D3DCommandList)));
	
		m_UploadBuffer = CreateScope<FUploadBuffer>();

		m_ResourceStateTracker = CreateScope<FResourceStateTracker>();

		for (int i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i)
		{
			m_DescriptorHeaps[i] = nullptr;
		}
	}

	void FDX12CommandList::FlushResourceBarriers()
	{
		m_ResourceStateTracker->FlushResourceBarriers(shared_from_this());
	}

	ComPtr<ID3D12Resource> FDX12CommandList::CreateTextureResource(Ref<FImage> Image)
	{
		ID3D12Device* D3DDevice = FDX12Device::Get()->GetDevice();
		ComPtr<ID3D12Resource> TextureResource;

		D3D12_RESOURCE_DESC TextureDesc = {};
		TextureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		TextureDesc.Alignment = 0;
		TextureDesc.Width = Image->GetWidth();
		TextureDesc.Height = Image->GetHeight();
		TextureDesc.DepthOrArraySize = 1;
		TextureDesc.MipLevels = 1;
		TextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		TextureDesc.SampleDesc.Count = 1;
		TextureDesc.SampleDesc.Quality = 0;
		TextureDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		TextureDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS;

		ThrowIfFailed(
			D3DDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, &TextureDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&TextureResource))
		);
	
		D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint;
		UINT64  TotalBytes = 0;
		FDX12Device::Get()->GetDevice()->GetCopyableFootprints(&TextureDesc, 0, 1, 0, &footprint, nullptr, nullptr, &TotalBytes);

		D3D12_RESOURCE_DESC UploadTexDesc;
		memset(&UploadTexDesc, 0, sizeof(UploadTexDesc));
		UploadTexDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		UploadTexDesc.Width = TotalBytes;
		UploadTexDesc.Height = 1;
		UploadTexDesc.DepthOrArraySize = 1;
		UploadTexDesc.MipLevels = 1;
		UploadTexDesc.SampleDesc.Count = 1;
		UploadTexDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		ComPtr<ID3D12Resource> UploadResource;
		ThrowIfFailed(D3DDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
			&UploadTexDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
			IID_PPV_ARGS(&UploadResource))
		);

		TransitionBarrier(TextureResource, D3D12_RESOURCE_STATE_COPY_DEST);
		FlushResourceBarriers();

		D3D12_SUBRESOURCE_DATA SubResourceData = {};
		SubResourceData.pData = Image->GetData();
		SubResourceData.RowPitch = Image->GetWidth() * Image->GetChannel();
		SubResourceData.SlicePitch = TotalBytes;

		UpdateSubresources<1>(m_D3DCommandList.Get(), TextureResource.Get(), UploadResource.Get(), 0, 0, 1, &SubResourceData);

		TransitionBarrier(TextureResource.Get(), D3D12_RESOURCE_STATE_GENERIC_READ);
		TrackResource(UploadResource);
		TrackResource(TextureResource);

		return TextureResource;
	}

	ComPtr<ID3D12Resource> FDX12CommandList::CreateRenderTargetResource(uint32_t Width, uint32_t Height)
	{
		ID3D12Device* D3DDevice = FDX12Device::Get()->GetDevice();

		D3D12_RESOURCE_DESC RtvResourceDesc;
		RtvResourceDesc.Alignment = 0;
		RtvResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		RtvResourceDesc.DepthOrArraySize = 1;
		RtvResourceDesc.Width = Width;
		RtvResourceDesc.Height = Height;
		RtvResourceDesc.MipLevels = 1;
		RtvResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		RtvResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		RtvResourceDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		RtvResourceDesc.SampleDesc.Count = 1;
		RtvResourceDesc.SampleDesc.Quality = 0;

		CD3DX12_CLEAR_VALUE OptClear;
		OptClear.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		memcpy(OptClear.Color, DirectX::Colors::Transparent, 4 * sizeof(float));


		ComPtr<ID3D12Resource> Resource;
	
		ThrowIfFailed(D3DDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&RtvResourceDesc,
			D3D12_RESOURCE_STATE_COMMON,
			&OptClear,
			IID_PPV_ARGS(&Resource))
		);
		TransitionBarrier(Resource, D3D12_RESOURCE_STATE_RENDER_TARGET);
		
		return Resource;
	}

	void FDX12CommandList::ResolveSubResource(const Ref<IResource>& DstRes, const Ref<IResource> SrcRes, uint32_t DstSubRes, uint32_t SrcSubRes)
	{
		TransitionBarrier(DstRes, D3D12_RESOURCE_STATE_RESOLVE_DEST, DstSubRes);
		TransitionBarrier(SrcRes, D3D12_RESOURCE_STATE_RESOLVE_SOURCE, SrcSubRes);
	
		FlushResourceBarriers();

		m_D3DCommandList->ResolveSubresource(DstRes->GetD3DResource().Get(), DstSubRes,
			SrcRes->GetD3DResource().Get(), SrcSubRes,
			DstRes->GetD3D12ResourceDesc().Format);

		TrackResource(SrcRes);
		TrackResource(DstRes);
	}

	void FDX12CommandList::ClearTexture(FDX12Texture2D* TexturePtr, const ZMath::vec4 Color)
	{
		TransitionBarrier(TexturePtr->GetD3DResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, true);
		float ClearColor[4] = { Color.r, Color.g, Color.b, Color.a };
		m_D3DCommandList->ClearRenderTargetView(TexturePtr->GetRenderTargetView(), ClearColor, 0, nullptr);

		TrackResource(TexturePtr->GetD3DResource());
	}

	void FDX12CommandList::ClearDepthStencilTexture(FDX12Texture2D* TexturePtr, D3D12_CLEAR_FLAGS ClearFlags, float Depth, uint8_t Stencil)
	{
		TransitionBarrier(TexturePtr->GetD3DResource(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, true);
		m_D3DCommandList->ClearDepthStencilView(TexturePtr->GetDepthStencilView(), ClearFlags, Depth, Stencil, 0, nullptr);

		TrackResource(TexturePtr->GetD3DResource());
	}

	void FDX12CommandList::CopyResource(const Ref<IResource>& DstRes, const Ref<IResource>& SrcRes)
	{
		CopyResource(DstRes->GetD3DResource(), SrcRes->GetD3DResource());
	}

	void FDX12CommandList::CopyResource(ComPtr<ID3D12Resource> DstRes, ComPtr<ID3D12Resource> SrcRes)
	{
		TransitionBarrier(DstRes, D3D12_RESOURCE_STATE_COPY_DEST);
		TransitionBarrier(SrcRes, D3D12_RESOURCE_STATE_COPY_SOURCE);

		FlushResourceBarriers();

		m_D3DCommandList->CopyResource(DstRes.Get(), SrcRes.Get());

		TrackResource(DstRes);
		TrackResource(SrcRes);
	}

	void FDX12CommandList::Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t startVertex, uint32_t startInstance)
	{
		
	}
	
	void FDX12CommandList::DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t startIndex, int32_t baseVertex,
						  uint32_t startInstance)
	{
		
	}
			
	void FDX12CommandList::SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE HeapType, ID3D12DescriptorHeap* Heap)
	{
		if (m_DescriptorHeaps[HeapType] != Heap)
		{
			m_DescriptorHeaps[HeapType] = Heap;
			BindDescriptorHeaps();
		}
	}

	void FDX12CommandList::BindDescriptorHeaps()
	{
		UINT NumDescriptorHeaps = 0;
		ID3D12DescriptorHeap* DescriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES] = {};
		for (uint32_t i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i)
		{
			ID3D12DescriptorHeap* DescriptorHeap = m_DescriptorHeaps[i];
			if (DescriptorHeap)
			{
				DescriptorHeaps[NumDescriptorHeaps++] = DescriptorHeap;
			}
		}

		m_D3DCommandList->SetDescriptorHeaps(NumDescriptorHeaps, DescriptorHeaps);
	}

	void FDX12CommandList::SetPipelineState(const Ref<FPipelineStateObject>& PipelineState)
	{
		auto D3DPipelineStateObj = PipelineState->GetD3D12PipelineState().Get();
		if (m_PipelineState != D3DPipelineStateObj)
		{
			m_PipelineState = D3DPipelineStateObj;
			m_D3DCommandList->SetPipelineState(D3DPipelineStateObj);

			TrackResource(D3DPipelineStateObj);
		}
	}

	void FDX12CommandList::SetRenderTarget(const FDX12RenderTarget& RenderTarget)
	{
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> RenderTargetDescriptors;
		RenderTargetDescriptors.reserve(EAttachmentIndex::NumAttachmentPoints);
		
		const auto& Textures = RenderTarget.GetTextures();
		
		for (int i = EAttachmentIndex::Color0; i <= EAttachmentIndex::Color7; ++i)
		{
			auto* Texture = static_cast<FDX12Texture2D*>(Textures[i].get());
			if (Texture)
			{
				TransitionBarrier(Texture->GetD3DResource(), D3D12_RESOURCE_STATE_RENDER_TARGET);
				RenderTargetDescriptors.push_back(Texture->GetRenderTargetView());

				TrackResource(Texture->GetD3DResource());
			}
		}
		
		auto* DepthTexture = static_cast<FDX12Texture2D*>(RenderTarget.GetTexture(EAttachmentIndex::DepthStencil).get());
		
		CD3DX12_CPU_DESCRIPTOR_HANDLE DepthStencilDescriptor(D3D12_DEFAULT);
		if (DepthTexture)
		{
			TransitionBarrier(DepthTexture->GetD3DResource(), D3D12_RESOURCE_STATE_DEPTH_WRITE);
			DepthStencilDescriptor = DepthTexture->GetDepthStencilView();
			TrackResource(DepthTexture->GetD3DResource());
		}

		D3D12_CPU_DESCRIPTOR_HANDLE* pDSV = DepthStencilDescriptor.ptr != 0 ? &DepthStencilDescriptor : nullptr;
		
		m_D3DCommandList->OMSetRenderTargets(static_cast<UINT>(RenderTargetDescriptors.size()),
			RenderTargetDescriptors.data(), FALSE, pDSV);
	}

	void FDX12CommandList::TransitionBarrier(const Ref<IResource>& Resource, D3D12_RESOURCE_STATES StateAfter, UINT Subresource, bool bFlushBarriers)
	{
		if (Resource)
		{
			TransitionBarrier(Resource->GetD3DResource(), StateAfter, Subresource, bFlushBarriers);
		}
	}

	void FDX12CommandList::TransitionBarrier(ComPtr<ID3D12Resource> Resource, D3D12_RESOURCE_STATES StateAfter, UINT Subresource, bool bFlushBarriers)
	{
		if (Resource)
		{
			auto Barrier = CD3DX12_RESOURCE_BARRIER::Transition(Resource.Get(), D3D12_RESOURCE_STATE_COMMON, StateAfter, Subresource);
			m_ResourceStateTracker->ResourceBarrier(Barrier);
		}
		
		if (bFlushBarriers)
		{
			FlushResourceBarriers();
		}
	}

	void FDX12CommandList::TrackResource(Microsoft::WRL::ComPtr<ID3D12Object> Object)
	{
		m_TrackedObjects.push_back(Object);
	}

	void FDX12CommandList::TrackResource(const Ref<IResource>& res)
	{
		TrackResource(res->GetD3DResource());
	}

	bool FDX12CommandList::Close(const Ref<FDX12CommandList>& PendingCommandList)
	{
		FlushResourceBarriers();
		
		m_D3DCommandList->Close();
		
		uint32_t NumPendingBarriers = m_ResourceStateTracker->FlushPendingResourceBarriers(PendingCommandList);
		
		m_ResourceStateTracker->CommitFinalResourceStates();
		
		return NumPendingBarriers > 0;
	}

	void FDX12CommandList::Close()
	{
		FlushResourceBarriers();
		m_D3DCommandList->Close();
	}

	ComPtr<ID3D12Resource> FDX12CommandList::CreateDefaultBuffer(const void* BufferData, size_t BufferSize, D3D12_RESOURCE_FLAGS Flags)
	{
		CORE_ASSERT(BufferSize != 0 && BufferData != nullptr, "InValid Buffer!")
		ComPtr<ID3D12Resource> D3DResource;
		
		auto  D3DDevice = FDX12Device::Get()->GetDevice();

		ThrowIfFailed(D3DDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(BufferSize), D3D12_RESOURCE_STATE_COMMON, nullptr,
			IID_PPV_ARGS(D3DResource.GetAddressOf()))
		);

		ComPtr<ID3D12Resource> UploadResource;
		ThrowIfFailed(D3DDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(BufferSize), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
			IID_PPV_ARGS(UploadResource.GetAddressOf()))
		);

		D3D12_SUBRESOURCE_DATA SubresourceData = {};
		SubresourceData.pData = BufferData;
		SubresourceData.RowPitch = BufferSize;
		SubresourceData.SlicePitch = SubresourceData.RowPitch;

		m_ResourceStateTracker->TransitionResource(D3DResource.Get(), D3D12_RESOURCE_STATE_COPY_DEST);
		FlushResourceBarriers();

		UpdateSubresources<1>(m_D3DCommandList.Get(), D3DResource.Get(), UploadResource.Get(), 0, 0, 1, &SubresourceData);
		m_ResourceStateTracker->TransitionResource(D3DResource.Get(), D3D12_RESOURCE_STATE_COMMON);

		TrackResource(UploadResource);
		TrackResource(D3DResource);

		return D3DResource;
	}

	void FDX12CommandList::SetGraphicsRootSignature(const Ref<IRootSignature>& RootSignature)
	{
		
		FDX12RootSignature* DX12RootSignature = static_cast<FDX12RootSignature*>(RootSignature.get());
		auto* D3DRootSignature = DX12RootSignature->GetD3D12RootSignature().Get();
		if (D3DRootSignature != m_RootSignature)
		{
			m_RootSignature = D3DRootSignature;
			m_D3DCommandList->SetGraphicsRootSignature(m_RootSignature);
			TrackResource(m_RootSignature);
		}
	}

	void FDX12CommandList::Reset()
	{
		ThrowIfFailed(m_CommandAllocator->Reset());
		ThrowIfFailed(m_D3DCommandList->Reset(m_CommandAllocator.Get(), nullptr));
		
		m_ResourceStateTracker->Reset();
		m_UploadBuffer->Reset();

		ReleaseTrackedObjects();
		
		for (int i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i)
		{
			m_DescriptorHeaps[i] = nullptr;
		}
		
		m_RootSignature = nullptr;
	}
	

	void FDX12CommandList::ReleaseTrackedObjects()
	{
		m_TrackedObjects.clear();
	}
	
	
	void FDX12CommandList::Execute()
	{
	}
}
