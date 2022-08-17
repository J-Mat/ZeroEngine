#include "DX12CommandList.h"

namespace Zero
{
	FDX12CommandList::FDX12CommandList(FDX12Device& InDevice, D3D12_COMMAND_LIST_TYPE Type)
	: m_Device(InDevice), 
	m_CommandListType(Type)
	{
		ThrowIfFailed(m_Device.GetDevice()->CreateCommandAllocator(m_CommandListType, IID_PPV_ARGS(&m_CommandAllocator)));

		ThrowIfFailed(m_Device.GetDevice()->CreateCommandList(0, m_CommandListType, m_CommandAllocator.Get(),
			nullptr, IID_PPV_ARGS(&m_D3DCommandList)));
	
		m_UploadBuffer = CreateScope<FUploadBuffer>(m_Device);

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

	ComPtr<ID3D12Resource> FDX12CommandList::CreateDefaultBuffer(const void* BufferData, size_t BufferSize, D3D12_RESOURCE_FLAGS Flags)
	{
		CORE_ASSERT(BufferSize != 0 && BufferData != nullptr, "InValid Buffer!")
		ComPtr<ID3D12Resource> D3DResource;

		ID3D12Device* D3DDevice = m_Device.GetDevice();

		ThrowIfFailed(D3DDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(BufferSize, Flags), D3D12_RESOURCE_STATE_COMMON, nullptr,
			IID_PPV_ARGS(&D3DResource))
		);
		FResourceStateTracker::AddGlobalResourceState(D3DResource.Get(), D3D12_RESOURCE_STATE_COMMON);

		ComPtr<ID3D12Resource> UploadResource;
		ThrowIfFailed(D3DDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(BufferSize), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
			IID_PPV_ARGS(&UploadResource))
		);
		D3D12_SUBRESOURCE_DATA SubresourceData = {};
		SubresourceData.pData = BufferData;
		SubresourceData.RowPitch = BufferSize;
		SubresourceData.SlicePitch = SubresourceData.RowPitch;
		
		m_ResourceStateTracker->TransitionResource(D3DResource.Get(), D3D12_RESOURCE_STATE_COPY_DEST);
		FlushResourceBarriers();

		UpdateSubresources(m_D3DCommandList.Get(), D3DResource.Get(), UploadResource.Get(), 0, 0, 1,
			&SubresourceData);

		
		return ComPtr<ID3D12Resource>();
	}

	ComPtr<ID3D12Resource> FDX12CommandList::CreateTextureResource(Ref<FImage> Image)
	{
		ID3D12Device* D3DDevice = m_Device.GetDevice();
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

		TransitionBarrier(TextureResource, D3D12_RESOURCE_STATE_COPY_DEST);
		FlushResourceBarriers();

		ComPtr<ID3D12Resource> IntermediateResource;
		ThrowIfFailed(D3DDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(Image->GetBufferSize()), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
			IID_PPV_ARGS(&IntermediateResource))
		);

		D3D12_SUBRESOURCE_DATA SubResourceData = {};
		SubResourceData.pData = Image->GetData();
		SubResourceData.RowPitch = Image->GetWidth() * 4;
		SubResourceData.SlicePitch = Image->GetBufferSize();

		UpdateSubresources<1>(m_D3DCommandList.Get(), TextureResource.Get(), IntermediateResource.Get(), 0, 0, 1, & SubResourceData);

		TrackResource(IntermediateResource);
		TrackResource(TextureResource);

		return TextureResource;
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
			auto Barrier = CD3DX12_RESOURCE_BARRIER::Transition(Resource.Get(), D3D12_RESOURCE_STATE_COMMON, StateAfter);
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

	void FDX12CommandList::Reset()
	{
		ThrowIfFailed(m_CommandAllocator->Reset());
		ThrowIfFailed(m_D3DCommandList->Reset(m_CommandAllocator.Get(), nullptr));
		
		m_ResourceStateTracker->Reset();
		m_UploadBuffer->Reset();

		ReleaseTrackedObjects();
		
		//#todo
	}
	
	
	void FDX12CommandList::Execute()
	{
	}
}
