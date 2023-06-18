#include "DX12CommandList.h"
#include "DX12Texture2D.h"
#include "PSO/DX12GraphicPipelineStateObject.h"
#include "DX12RenderTarget2D.h"
#include "DX12RootSignature.h"
#include "./PSO/GenerateMipsPSO.h"
#include "./ResourceView/DX12RenderTargetView.h"
#include "Utils.h"
#include "Core/Framework/Library.h"

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

		for (uint32_t i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i)
		{
			m_DescriptorHeaps[i] = nullptr;
			
			m_DynamicDescriptorHeap[i] = CreateScope<FDynamicDescriptorHeap>(D3D12_DESCRIPTOR_HEAP_TYPE(i));
		}
		m_DescriptorCache = CreateScope<FDescriptorCache>();
	}

	void FDX12CommandList::Init()
	{
	}


	void FDX12CommandList::FlushResourceBarriers()
	{
		m_ResourceStateTracker->FlushResourceBarriers(shared_from_this());
	}

	void FDX12CommandList::OnDeployed()
	{
		if (m_CommandListType != D3D12_COMMAND_LIST_TYPE_COPY)
		{
			SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, m_DescriptorCache->GetCacheCbvSrvUavDescriptorHeap().Get());
		}	
		//SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, m_DescriptorCache->GetCacheRtvDescriptorHeap().Get());
	}

	Ref<FDX12Resource> FDX12CommandList::CreateTextureResource(const std::string& TextureName, Ref<FImage> Image, bool bGenerateMip)
	{
		ID3D12Device* D3DDevice = FDX12Device::Get()->GetDevice();
		ComPtr<ID3D12Resource> TextureD3DResource;

		uint32_t Mips = 1;
		if (bGenerateMip)
		{
			Mips = ZMath::min(ZMath::CalLog2Interger(Image->GetWidth()), ZMath::CalLog2Interger(Image->GetHeight()));
		}
		
		D3D12_RESOURCE_DESC TextureDesc = {};
		TextureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		TextureDesc.Alignment = 0;
		TextureDesc.Width = Image->GetWidth();
		TextureDesc.Height = Image->GetHeight();
		TextureDesc.DepthOrArraySize = 1;
		TextureDesc.MipLevels = Mips;
		TextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		TextureDesc.SampleDesc.Count = 1;
		TextureDesc.SampleDesc.Quality = 0;
		TextureDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		TextureDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS;

		ThrowIfFailed(
			D3DDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, &TextureDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&TextureD3DResource))
		);

		D3D12_PLACED_SUBRESOURCE_FOOTPRINT Footprint;
		UINT64  TotalBytes = 0;
		FDX12Device::Get()->GetDevice()->GetCopyableFootprints(&TextureDesc, 0, 1, 0, &Footprint, nullptr, nullptr, &TotalBytes);

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

		TransitionBarrier(TextureD3DResource, D3D12_RESOURCE_STATE_COPY_DEST);
		FlushResourceBarriers();

		D3D12_SUBRESOURCE_DATA SubResourceData = {};
		SubResourceData.pData = Image->GetData();
		SubResourceData.RowPitch = Image->GetWidth() * Image->GetChannel();
		SubResourceData.SlicePitch = TotalBytes;

		UpdateSubresources(m_D3DCommandList.Get(), TextureD3DResource.Get(), UploadResource.Get(), 0, 0, 1, &SubResourceData);

		TrackResource(UploadResource);
		TrackResource(TextureD3DResource);
		
		Ref<FDX12Resource> TextureResource = CreateRef<FDX12Resource>(TextureName, TextureD3DResource);

		TransitionBarrier(TextureD3DResource.Get(), D3D12_RESOURCE_STATE_COMMON);

		return TextureResource;
	}

	void FDX12CommandList::AllocateTextureResource(const std::string& TextureName, const FTextureDesc& TextureDesc, FResourceLocation& ResourceLocation, Ref<FImage> Image /*= nullptr*/, bool bGenerateMip /*= false*/)
	{
		D3D12_RESOURCE_DESC ResourceDesc = FDX12Utils::ConvertResourceDesc(TextureDesc);

		auto* TextureResourceAllocator = FDX12Device::Get()->GetTextureResourceAllocator();
		TextureResourceAllocator->AllocTextureResource(TextureName, ResourceDesc, ResourceLocation);

		if (Image != nullptr)
		{
			D3D12_PLACED_SUBRESOURCE_FOOTPRINT Footprint;
			UINT64  TotalBytes = 0;
			FDX12Device::Get()->GetDevice()->GetCopyableFootprints(&ResourceDesc, 0, 1, 0, &Footprint, nullptr, nullptr, &TotalBytes);

			auto* UploadResourceAllocator = FDX12Device::Get()->GetUploadBufferAllocator();
			FResourceLocation UploadResourceLocation;
			void* MappedData = UploadResourceAllocator->AllocUploadResource(TotalBytes, UPLOAD_RESOURCE_ALIGNMENT, UploadResourceLocation);
			ID3D12Resource* UploadBuffer = UploadResourceLocation.m_UnderlyingResource->GetD3DResource().Get();

			D3D12_PLACED_SUBRESOURCE_FOOTPRINT Layout;
			uint64_t RequiredSize = 0;
			uint32_t NumRow;
			uint64_t RowSizesInBytes;
			ID3D12Device* D3DDevice = FDX12Device::Get()->GetDevice();
			D3DDevice->GetCopyableFootprints(&ResourceDesc, 0, 1, 0, &Layout, &NumRow, &RowSizesInBytes, &RequiredSize);


			D3D12_SUBRESOURCE_DATA InitData = {};
			InitData.pData = Image->GetData();
			InitData.RowPitch = Image->GetWidth() * Image->GetChannel();
			InitData.SlicePitch = TotalBytes;

			D3D12_MEMCPY_DEST DestData = { (BYTE*)MappedData + Layout.Offset, Layout.Footprint.RowPitch, SIZE_T(Layout.Footprint.RowPitch) * SIZE_T(NumRow) };
			MemcpySubresource(&DestData, &InitData, static_cast<SIZE_T>(RowSizesInBytes), NumRow, Layout.Footprint.Depth);

			TransitionBarrier(ResourceLocation.m_UnderlyingResource, D3D12_RESOURCE_STATE_COPY_DEST);

			Layout.Offset += UploadResourceLocation.m_OffsetFromBaseOfResource;

			CD3DX12_TEXTURE_COPY_LOCATION Src;
			Src.pResource = UploadBuffer;
			Src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
			Src.PlacedFootprint = Layout;

			CD3DX12_TEXTURE_COPY_LOCATION Dst;
			Dst.pResource = ResourceLocation.m_UnderlyingResource->GetD3DResource().Get();
			Dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
			Dst.SubresourceIndex = 0;

			CopyTextureRegion(&Dst, 0, 0, 0, &Src, nullptr);
		}
	}

	void FDX12CommandList::AllocateTextureCubemapResource(const std::string& TextureName, const FTextureDesc& TextureDesc, FResourceLocation& ResourceLocation, Ref<FImage> ImageData[CUBEMAP_TEXTURE_CNT])
	{
		ID3D12Device* D3DDevice = FDX12Device::Get()->GetDevice();
		D3D12_RESOURCE_DESC ResourceDesc = FDX12Utils::ConvertResourceDesc(TextureDesc);

		auto* TextureResourceAllocator = FDX12Device::Get()->GetTextureResourceAllocator();
		TextureResourceAllocator->AllocTextureResource(TextureName, ResourceDesc, ResourceLocation);

		UINT NumSubresources = CUBEMAP_TEXTURE_CNT;
		D3D12_PLACED_SUBRESOURCE_FOOTPRINT  Footprint[CUBEMAP_TEXTURE_CNT] = {};
		UINT NumRows[CUBEMAP_TEXTURE_CNT] = {};
		UINT64  n64RowSizeInBytes[CUBEMAP_TEXTURE_CNT] = {};
		UINT64  TotalBytes = 0;
		// 获取资源内存布局信息
		D3DDevice->GetCopyableFootprints(&ResourceDesc,
			0,
			NumSubresources,
			0,
			Footprint,
			NumRows,
			n64RowSizeInBytes,
			&TotalBytes);

		
		UINT64 UploadSize = GetRequiredIntermediateSize(
			ResourceLocation.GetResource()->GetD3DResource().Get(),
			0,
			CUBEMAP_TEXTURE_CNT);

		auto* UploadResourceAllocator = FDX12Device::Get()->GetUploadBufferAllocator();
		FResourceLocation UploadResourceLocation;
		void* MappedData = UploadResourceAllocator->AllocUploadResource(UploadSize, UPLOAD_RESOURCE_ALIGNMENT, UploadResourceLocation);
		ID3D12Resource* UploadBuffer = UploadResourceLocation.m_UnderlyingResource->GetD3DResource().Get();


		TransitionBarrier(ResourceLocation.GetResource(), D3D12_RESOURCE_STATE_COPY_DEST);
		FlushResourceBarriers();

		D3D12_SUBRESOURCE_DATA SubResourceData[CUBEMAP_TEXTURE_CNT] = {};
		for (int i = 0; i < CUBEMAP_TEXTURE_CNT; ++i)
		{
			SubResourceData[i].pData = ImageData[i]->GetData();
			SubResourceData[i].RowPitch = ImageData[i]->GetWidth() * ImageData[i]->GetChannel();
			SubResourceData[i].SlicePitch = ImageData[i]->GetHeight() * SubResourceData[i].RowPitch;
		}

		UpdateSubresources(
			m_D3DCommandList.Get(), 
			ResourceLocation.GetResource()->GetD3DResource().Get(),
			UploadResourceLocation.GetResource()->GetD3DResource().Get(), 
			0, 
			0, 
			CUBEMAP_TEXTURE_CNT, 
			SubResourceData);

		TransitionBarrier(ResourceLocation.GetResource(), D3D12_RESOURCE_STATE_GENERIC_READ);
		TrackResource(ResourceLocation.GetResource());
	}

	void FDX12CommandList::GenerateMip(Ref<FDX12Resource> TextureResource)
	{
		CORE_ASSERT(m_CommandListType == D3D12_COMMAND_LIST_TYPE_COMPUTE, "ComandList must be compute");

		ID3D12Device* D3DDevice = FDX12Device::Get()->GetDevice();
		
		auto TextureD3DResourece = TextureResource->GetD3DResource();
		auto ResourceDesc = TextureD3DResourece->GetDesc();
		if (ResourceDesc.Dimension != D3D12_RESOURCE_DIMENSION_TEXTURE2D || ResourceDesc.DepthOrArraySize != 1 || ResourceDesc.SampleDesc.Count > 1)
		{
			throw std::exception("GenerateMips is only supported for non-multi-sampled 2D Textures.");
		}

		ComPtr<ID3D12Resource> UavResourece = TextureD3DResourece;
		// Create an alias of the original resource.
		// This is done to perform a GPU copy of resources with different formats.
		// BGR -> RGB texture copies will fail GPU validation unless performed
		// through an alias of the BRG resource in a placed heap.
		ComPtr<ID3D12Resource> AliasResourece;

		// If the passed-in resource does not allow for UAV access
		// then create a staging resource that is used to generate
		// the mipmap chain.	
		if (!TextureResource->CheckUAVSupport() || (ResourceDesc.Flags & D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS) == 0)
		{
			// Describe an alias resource that is used to copy the original texture.
			D3D12_RESOURCE_DESC AliasDesc = ResourceDesc;
			// Placed resources can't be render targets or Depth-Stencil views.
			AliasDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
			AliasDesc.Flags &= ~(D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
			

			// Describe a UAV compatible resource that is used to perform
			// mipmapping of the original texture.
			auto UavDesc = AliasDesc;  // The flags for the UAV description must match that of the alias description. 
			UavDesc.Format = FDX12Texture2D::GetUAVCompatableFormat(ResourceDesc.Format);

			D3D12_RESOURCE_DESC ResourceDescs[] = { AliasDesc, UavDesc };

			// Create a heap that is large enough to store a copy of the original resource.
			auto AllocationInfo = D3DDevice->GetResourceAllocationInfo(0, _countof(ResourceDescs), ResourceDescs);

			D3D12_HEAP_DESC HeapDesc
			{
				.SizeInBytes = AllocationInfo.SizeInBytes,
				.Properties = {
					.Type = D3D12_HEAP_TYPE_DEFAULT,
					.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
					.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN,
				},
				.Alignment = AllocationInfo.Alignment,
				.Flags = D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES,
			};

			ComPtr<ID3D12Heap> Heap;
			ThrowIfFailed(D3DDevice->CreateHeap(&HeapDesc, IID_PPV_ARGS(&Heap)));

			// Make sure the heap does not go out of scope until the command list
			// is finished executing on the command queue.
			TrackResource(Heap);

			// Create a placed resource that matches the description of the
			// original resource. This resource is used to copy the original
			// texture to the UAV compatible resource.

			ThrowIfFailed(D3DDevice->CreatePlacedResource(Heap.Get(), 0, &AliasDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&AliasResourece)));

			FResourceStateTracker::AddGlobalResourceState(AliasResourece.Get(), D3D12_RESOURCE_STATE_COMMON);
			// Ensure the scope of the alias resource.
			TrackResource(AliasResourece);


			// Create a UAV compatible resource in the same heap as the alias
			// resource.
			ThrowIfFailed(D3DDevice->CreatePlacedResource(Heap.Get(), 0, &UavDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&UavResourece)));
			FResourceStateTracker::AddGlobalResourceState(UavResourece.Get(), D3D12_RESOURCE_STATE_COMMON);
			
			TrackResource(UavResourece);

			// Add an aliasing barrier for the alias resource.
			AliasingBarrier(nullptr, AliasResourece);

			// Copy the original resource to the alias resource.
			// This ensures GPU validation.
			CopyResource(AliasResourece, TextureD3DResourece);
			
			// Add an aliasing barrier for the UAV compatible resource.
			AliasingBarrier(AliasResourece, UavResourece);
		}

		uint32_t Width = (uint32_t)TextureD3DResourece->GetDesc().Width;
		uint32_t Height = TextureD3DResourece->GetDesc().Height;
		auto UavTexture = CreateRef<FDX12Texture2D>(std::format("{0}_UAVTexture", Utils::WString2String(TextureResource->GetName())), UavResourece, Width, Height);
		GenerateMips_UAV(UavTexture, FDX12Texture2D::IsSRGBFormat(ResourceDesc.Format));

		if (AliasResourece)
		{
			AliasingBarrier(UavResourece, AliasResourece);
			// Copy the alias resource back to the original resource.
			CopyResource(TextureD3DResourece, AliasResourece);
		}
	}

	void FDX12CommandList::GenerateMipSimple(Ref<FDX12Resource> TextureResource)
	{
		CORE_ASSERT(m_CommandListType == D3D12_COMMAND_LIST_TYPE_COMPUTE, "ComandList must be compute");

		ID3D12Device* D3DDevice = FDX12Device::Get()->GetDevice();

		Ref<FBuffer> ReadbackBuffer = TLibrary<FBuffer>::Fetch("ReadbackBuffer");
		if (ReadbackBuffer->IsCopy())
		{
			return;
		}
		if (m_GenerateMipsPSO == nullptr)
		{
			m_GenerateMipsPSO = CreateScope<FGenerateMipsPSO>();
		}
		SetPipelineState(m_GenerateMipsPSO->GetPipelineState());
		SetComputeRootSignature(m_GenerateMipsPSO->GetRootSignature());
		
		Ref<FBuffer> UavBuffer = TLibrary<FBuffer>::Fetch("UavBuffer");
		FDX12Buffer* D3DBuffer = static_cast<FDX12Buffer*>(UavBuffer.get());
		FDX12UnorderedAccessResourceView* Uav = static_cast<FDX12UnorderedAccessResourceView*>(D3DBuffer->GetUAV());

		CD3DX12_GPU_DESCRIPTOR_HANDLE GpuHandle = AppendCbvSrvUavDescriptors(&Uav->GetDescriptorHandle(), 1);
		m_D3DCommandList->SetComputeRootDescriptorTable(0, GpuHandle);

		m_D3DCommandList->Dispatch(1, 1, 1);
			
		CopyResource(ReadbackBuffer->GetNative(), UavBuffer->GetNative());
		TransitionBarrier(ReadbackBuffer->GetNative(), EResourceState::Common);
		ReadbackBuffer->SetCopy(true);

		/*
		D3D12_SHADER_RESOURCE_VIEW_DESC SrvDesc
		{
			.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D,
			.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
		};

		//auto Srv = CreateRef<FShaderResourceView>(TextureResource, &SrvDesc);

		D3D12_UNORDERED_ACCESS_VIEW_DESC UavDesc
		{
			.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D,
		};

		//auto Uav = CreateRef<FDX12UnorderedAccessResourceView>(TextureResource, &UavDesc);

		D3D12_RESOURCE_DESC TexDesc = TextureResource->GetD3DResource()->GetDesc();
		UINT const MipmapLevels = TexDesc.MipLevels;

		for (UINT TopMip = 0; TopMip < MipmapLevels - 1; TopMip++)
		{
			UINT dst_width = (std::max)((UINT)TexDesc.Width >> (TopMip + 1), 1u);
			UINT dst_height = (std::max)(TexDesc.Height >> (TopMip + 1), 1u);

			FDescriptorAllocation Descriptor = FDX12Device::Get()->AllocateRuntimeDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 2);
			D3D12_CPU_DESCRIPTOR_HANDLE Handle1 = Descriptor.GetDescriptorHandle(0);
			SrvDesc.Format = TexDesc.Format;
			SrvDesc.Texture2D.MipLevels = 1;
			SrvDesc.Texture2D.MostDetailedMip = TopMip;
			D3DDevice->CreateShaderResourceView(TextureResource->GetD3DResource().Get(), &SrvDesc, Descriptor.GetDescriptorHandle());
			CD3DX12_GPU_DESCRIPTOR_HANDLE GpuHandle1 = AppendCbvSrvUavDescriptors(&Handle1, 1);
			m_D3DCommandList->SetComputeRootDescriptorTable(1, GpuHandle1);

			D3D12_CPU_DESCRIPTOR_HANDLE Handle2 = Descriptor.GetDescriptorHandle(1);
			UavDesc.Format = TexDesc.Format;
			UavDesc.Texture2D.MipSlice = TopMip + 1;
			D3DDevice->CreateUnorderedAccessView(TextureResource->GetD3DResource().Get(), nullptr, &UavDesc, Handle2);
			CD3DX12_GPU_DESCRIPTOR_HANDLE GpuHandle2 = AppendCbvSrvUavDescriptors(&Handle2, 1);
			m_D3DCommandList->SetComputeRootDescriptorTable(2, GpuHandle2);

			FGenerateMipsCBTest GenerateMipsCBTest
			{
				.TexelSize = {1.0f / dst_width, 1.0f / dst_height}
			};

			SetCompute32BitConstants(0, GenerateMipsCBTest);

			m_D3DCommandList->Dispatch((std::max)(dst_width / 8u, 1u), (std::max)(dst_height / 8u, 1u), 1);

			UAVBarrier(TextureResource, true);
		}
		TransitionBarrier(TextureResource, D3D12_RESOURCE_STATE_COMMON);
		*/
	}

	void FDX12CommandList::GenerateMips_UAV(Ref<FDX12Texture2D> Texture, bool bIsSRGB)
	{
		if (m_GenerateMipsPSO == nullptr)
		{
			m_GenerateMipsPSO = CreateScope<FGenerateMipsPSO>();
		}
		SetPipelineState(m_GenerateMipsPSO->GetPipelineState());
		SetComputeRootSignature(m_GenerateMipsPSO->GetRootSignature());

		FGenerateMipsCB GenerateMipsCB;
		GenerateMipsCB.bIsSRGB = true;//bIsSRGB;


		auto Resource = Texture->GetResource()->GetD3DResource();
		auto ResourceDesc = Resource->GetDesc();

		// Create an SRV that uses the format of the original texture.
		D3D12_SHADER_RESOURCE_VIEW_DESC SrvDesc
		{
			.Format = bIsSRGB ? FDX12Utils::GetSRGBFormat(ResourceDesc.Format) : ResourceDesc.Format,
			.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D,
			.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
			.Texture2D = {
				.MipLevels = ResourceDesc.MipLevels
			}
		};

		auto Srv = CreateRef<FDX12ShaderResourceView>(Texture->GetResource(), &SrvDesc);
		
		for (uint32_t SrcMip = 0; SrcMip < ResourceDesc.MipLevels - 1u;)
		{
			uint64_t SrcWidth = ResourceDesc.Width >> SrcMip;
			uint32_t SrcHeight = ResourceDesc.Height >> SrcMip;
			uint32_t DstWidth = static_cast<uint32_t>(SrcWidth >> 1);
			uint32_t DstHeight = SrcHeight >> 1;

			// 0b00(0): Both width and height are even.
			// 0b01(1): Width is odd, height is even.
			// 0b10(2): Width is even, height is odd.
			// 0b11(3): Both width and height are odd.
			GenerateMipsCB.SrcDimension = ((SrcHeight & 1) << 1) | (SrcWidth & 1);

			// How many mipmap levels to compute this pass (max 4 mips per pass)
			DWORD MipCount;

			// The number of times we can half the size of the texture and get
		    // exactly a 50% reduction in size.
		    // A 1 bit in the width or height indicates an odd dimension.
		    // The case where either the width or the height is exactly 1 is handled
		    // as a special case (as the dimension does not require reduction).
			_BitScanForward(&MipCount, (DstWidth == 1 ? DstHeight : DstWidth) | (DstHeight == 1 ? DstWidth : DstHeight));

			// Maximum number of mips to generate is 4.
			MipCount = std::min<DWORD>(4, MipCount + 1);
			// Maximum number of mips to generate is 4
			MipCount = (SrcMip + MipCount) >= ResourceDesc.MipLevels ? ResourceDesc.MipLevels - SrcMip - 1 : MipCount;

			// Dimensions should not reduce to 0.
			// This can happen if the width and height are not the same.
			DstWidth = std::max<DWORD>(1, DstWidth);
			DstHeight = std::max<DWORD>(1, DstHeight);

			GenerateMipsCB.SrcMipLevel = SrcMip;
			GenerateMipsCB.NumMipLevels = MipCount;
			GenerateMipsCB.TexelSize = { 1.0f / DstWidth, 1.0f / DstHeight };
			GenerateMipsCB.bIsSRGB = true;

			SetCompute32BitConstants(EGenerateMips::GM_GenerateMipsCB, GenerateMipsCB);
			
			SetShaderResourceView(EGenerateMips::GM_SrcMip, 0, Srv, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, SrcMip, 1);

			for (uint32_t Mip = 0; Mip < MipCount; ++Mip)
			{
				D3D12_UNORDERED_ACCESS_VIEW_DESC UavDesc
				{
					.Format = ResourceDesc.Format,
					.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D,
					.Texture2D = {
						.MipSlice = SrcMip + Mip + 1
					}
				};
				auto Uav = CreateRef<FDX12UnorderedAccessResourceView>(Texture->GetResource(), nullptr, &UavDesc);
				SetUnorderedAccessView(EGenerateMips::GM_OutMip, Mip, Uav, D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
					SrcMip + Mip + 1, 1);
			}

			// Pad any unused mip levels with a default UAV. Doing this keeps the DX12 runtime happy.s
			if (MipCount < 4)
			{
				m_DynamicDescriptorHeap[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->StageDescriptors(EGenerateMips::GM_OutMip, MipCount, 4 - MipCount, m_GenerateMipsPSO->GetDefaultUAV());
			}
			Dispatch(ZMath::DivideByMultiple(DstWidth, 8), ZMath::DivideByMultiple(DstHeight, 8), 1);
		
			UAVBarrier(Texture->GetResource(), true);
			
			SrcMip += MipCount;
		} 
	}


	ComPtr<ID3D12Resource> FDX12CommandList::CreateTextureCubemapResource(Ref<FImage> ImageData[CUBEMAP_TEXTURE_CNT], uint64_t Width, uint32_t Height, uint32_t Chanels)
	{
		ID3D12Device* D3DDevice = FDX12Device::Get()->GetDevice();
		ComPtr<ID3D12Resource> TextureCubemapResource;

		D3D12_RESOURCE_DESC TextureDesc = {};
		TextureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		TextureDesc.Alignment = 0;
		TextureDesc.Width = Width;
		TextureDesc.Height = Height;
		TextureDesc.DepthOrArraySize = CUBEMAP_TEXTURE_CNT;
		TextureDesc.MipLevels = 1;
		TextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		TextureDesc.SampleDesc.Count = 1;
		TextureDesc.SampleDesc.Quality = 0;
		TextureDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		TextureDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS;

		ThrowIfFailed(
			D3DDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, &TextureDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&TextureCubemapResource))
		);

		UINT NumSubresources = CUBEMAP_TEXTURE_CNT;
		D3D12_PLACED_SUBRESOURCE_FOOTPRINT  Footprint[CUBEMAP_TEXTURE_CNT] = {};
		UINT NumRows[CUBEMAP_TEXTURE_CNT] = {};
		UINT64  n64RowSizeInBytes[CUBEMAP_TEXTURE_CNT] = {};
		UINT64  TotalBytes = 0;
		// 获取资源内存布局信息
		D3DDevice->GetCopyableFootprints(&TextureDesc,
			0,
			NumSubresources,
			0,
			Footprint,
			NumRows,
			n64RowSizeInBytes,
			&TotalBytes);

		
		UINT64 UploadSize = GetRequiredIntermediateSize(
			TextureCubemapResource.Get(),
			0,
			CUBEMAP_TEXTURE_CNT);

		CD3DX12_HEAP_PROPERTIES HeapProps(D3D12_HEAP_TYPE_UPLOAD);
		CD3DX12_RESOURCE_DESC UploadTexDesc = CD3DX12_RESOURCE_DESC::Buffer(UploadSize);

		// 创建纹理上传缓冲区
		ComPtr<ID3D12Resource> UploadResource;
		ThrowIfFailed(
			D3DDevice->CreateCommittedResource(
			&HeapProps, 
			D3D12_HEAP_FLAG_NONE,
			&UploadTexDesc, 
			D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
			IID_PPV_ARGS(&UploadResource)
			)
		);

		TransitionBarrier(TextureCubemapResource, D3D12_RESOURCE_STATE_COPY_DEST);
		FlushResourceBarriers();

		D3D12_SUBRESOURCE_DATA SubResourceData[CUBEMAP_TEXTURE_CNT] = {};
		for (int i = 0; i < CUBEMAP_TEXTURE_CNT; ++i)
		{
			SubResourceData[i].pData = ImageData[i]->GetData();
			SubResourceData[i].RowPitch = ImageData[i]->GetWidth() * Chanels;
			SubResourceData[i].SlicePitch = ImageData[i]->GetHeight() * SubResourceData[i].RowPitch;
		}

		UpdateSubresources(m_D3DCommandList.Get(), TextureCubemapResource.Get(), UploadResource.Get(), 0, 0, CUBEMAP_TEXTURE_CNT, SubResourceData);

		TransitionBarrier(TextureCubemapResource.Get(), D3D12_RESOURCE_STATE_GENERIC_READ);
		TrackResource(UploadResource);
		TrackResource(TextureCubemapResource);

		
		return TextureCubemapResource;
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

	void FDX12CommandList::ResolveSubResource(const Ref<FDX12Resource>& DstRes, const Ref<FDX12Resource> SrcRes, uint32_t DstSubRes, uint32_t SrcSubRes)
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
		auto Resource = TexturePtr->GetResource();
		TransitionBarrier(Resource, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, true);
		float ClearColor[4] = { Color.r, Color.g, Color.b, Color.a };
		FDX12RenderTargetView* Rtv = static_cast<FDX12RenderTargetView*>(TexturePtr->GetRTV());
		m_D3DCommandList->ClearRenderTargetView(Rtv->GetDescriptorHandle(), ClearColor, 0, nullptr);

		TrackResource(Resource);
	}

	void FDX12CommandList::ClearDepthStencilTexture(FDX12Texture2D* TexturePtr, D3D12_CLEAR_FLAGS ClearFlags, float Depth, uint8_t Stencil)
	{
		auto Resource = TexturePtr->GetResource();
		TransitionBarrier(Resource, D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, true);
		FDX12DepthStencilView* Dsv = static_cast<FDX12DepthStencilView*>(TexturePtr->GetDSV());
		m_D3DCommandList->ClearDepthStencilView(Dsv->GetDescriptorHandle(), ClearFlags, Depth, Stencil, 0, nullptr);

		TrackResource(Resource);
	}

	void FDX12CommandList::CopyResource(const Ref<FDX12Resource>& DstRes, const Ref<FDX12Resource>& SrcRes)
	{
		CopyResource(DstRes->GetD3DResource().Get(), SrcRes->GetD3DResource().Get());
	}

	void FDX12CommandList::CopyResource(ID3D12Resource* DstRes, ID3D12Resource* SrcRes)
	{
		TransitionBarrier(DstRes, D3D12_RESOURCE_STATE_COPY_DEST);
		TransitionBarrier(SrcRes, D3D12_RESOURCE_STATE_COPY_SOURCE);

		FlushResourceBarriers();

		m_D3DCommandList->CopyResource(DstRes, SrcRes);

		TrackResource(DstRes);
		TrackResource(SrcRes);
	}

	void FDX12CommandList::CopyResource(void* Dst, void* Src)
	{
		CopyResource((ID3D12Resource*)Dst, (ID3D12Resource*)Src);
	}

	void FDX12CommandList::CopyResource(ComPtr<ID3D12Resource> DstRes, ComPtr<ID3D12Resource> SrcRes)
	{
		CopyResource(DstRes.Get(), SrcRes.Get());
	}

	void FDX12CommandList::Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t startVertex, uint32_t startInstance)
	{

	}

	void FDX12CommandList::DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t startIndex, int32_t baseVertex,
		uint32_t startInstance)
	{

	}

	void FDX12CommandList::Dispatch(uint32_t NumGroupsX, uint32_t NumGroupsY, uint32_t NumGroupsZ)
	{
		FlushResourceBarriers();	

		/*
		for (int i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i)
		{
			m_DynamicDescriptorHeap[i]->CommitStagedDescriptorsForDispatch(m_CommandListHandle);
		}
		*/
		m_D3DCommandList->Dispatch(NumGroupsX, NumGroupsY, NumGroupsZ);
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

	void FDX12CommandList::SetPipelineState(const Ref<FDX12GraphicPipelineStateObject>& PipelineState)
	{
		auto D3DPipelineStateObj = PipelineState->GetD3D12PipelineState();
		SetPipelineState(D3DPipelineStateObj);
	}

	void FDX12CommandList::SetPipelineState(ComPtr<ID3D12PipelineState> D3DPipelineState)
	{
		if (m_PipelineState != D3DPipelineState.Get())
		{
			m_PipelineState = D3DPipelineState.Get();
			TrackResource(m_PipelineState);
		}
		m_D3DCommandList->SetPipelineState(m_PipelineState);
	}

	void FDX12CommandList::TransitionBarrier(const Ref<FDX12Resource>& Resource, D3D12_RESOURCE_STATES StateAfter, UINT Subresource, bool bFlushBarriers)
	{
		if (Resource)
		{
			TransitionBarrier(Resource->GetD3DResource().Get(), StateAfter, Subresource, bFlushBarriers);
		}
	}

	void FDX12CommandList::TransitionBarrier(ComPtr<ID3D12Resource> Resource, D3D12_RESOURCE_STATES StateAfter, UINT Subresource, bool bFlushBarriers)
	{
		TransitionBarrier(Resource.Get(), StateAfter, Subresource, bFlushBarriers);
	}

	void FDX12CommandList::TransitionBarrier(ID3D12Resource* Resource, D3D12_RESOURCE_STATES StateAfter, UINT Subresource /*= D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES*/, bool bFlushBarriers /*= false*/)
	{
		if (Resource)
		{
			auto Barrier = CD3DX12_RESOURCE_BARRIER::Transition(Resource, D3D12_RESOURCE_STATE_COMMON, StateAfter, Subresource);
			m_ResourceStateTracker->ResourceBarrier(Barrier);
		}

		if (bFlushBarriers)
		{
			FlushResourceBarriers();
		}
	}


	void FDX12CommandList::TransitionBarrier(void* Resource, EResourceState ResourceState, UINT Subresource /*= -1*/, bool bFlushBarriers /*= false*/)
	{
		TransitionBarrier((ID3D12Resource*)Resource, FDX12Utils::ConvertToD3DResourceState(ResourceState), Subresource, bFlushBarriers);
	}

	void FDX12CommandList::UAVBarrier(const Ref<FDX12Resource>& Resource, bool bFlushBarriers)
	{
		auto D3DResource = Resource ? Resource->GetD3DResource() : nullptr;
		UAVBarrier(D3DResource, bFlushBarriers);
	}

	void FDX12CommandList::UAVBarrier(ComPtr<ID3D12Resource> Resource, bool bFlushBarriers)
	{
		auto Barrier = CD3DX12_RESOURCE_BARRIER::UAV(Resource.Get());

		m_ResourceStateTracker->ResourceBarrier(Barrier);

		if (bFlushBarriers)
		{
			FlushResourceBarriers();
		}
	}


	void FDX12CommandList::AliasingBarrier(const Ref<FDX12Resource>& BeforeResource, const Ref<FDX12Resource>& AfterResource, bool bFlushBarriers)
	{
		auto D3DBeforeResourece = BeforeResource ? BeforeResource->GetD3DResource() : nullptr;
		auto D3DAfterResourece = AfterResource ? AfterResource->GetD3DResource() : nullptr;

		AliasingBarrier(D3DBeforeResourece, D3DAfterResourece, bFlushBarriers);
	}

	void FDX12CommandList::AliasingBarrier(ComPtr<ID3D12Resource> BeforeResource, ComPtr<ID3D12Resource> AfterResource, bool bFlushBarriers)
	{
		auto Barrier = CD3DX12_RESOURCE_BARRIER::Aliasing(BeforeResource.Get(), AfterResource.Get());
		m_ResourceStateTracker->ResourceBarrier(Barrier);
		if (bFlushBarriers)
		{
			FlushResourceBarriers();
		}
	}

	void FDX12CommandList::SetShaderResourceView(uint32_t RootParameterIndex, uint32_t DescriptorOffset, const Ref<FDX12ShaderResourceView>& SRV, D3D12_RESOURCE_STATES StateAfter /*= D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE*/, UINT FirstSubresource /*= 0*/, UINT NumSubresource /*= D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES*/)
	{
		auto Resource = SRV->GetResource();
		if (NumSubresource < D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES)
		{
			for (uint32_t i = 0; i < NumSubresource; ++i)
			{
				TransitionBarrier(Resource, StateAfter, FirstSubresource + i);
			}
		}
		else
		{
			TransitionBarrier(Resource, StateAfter);
		}
		
		m_DynamicDescriptorHeap[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->StageDescriptors(
			RootParameterIndex,
			DescriptorOffset,
			1,
			SRV->GetDescriptorHandle()
		);
	}

	void FDX12CommandList::SetUnorderedAccessView(uint32_t RootParameterIndex, uint32_t DescriptorOffset, const Ref<FDX12UnorderedAccessResourceView>& Uav, D3D12_RESOURCE_STATES StateAfter /*= D3D12_RESOURCE_STATE_UNORDERED_ACCESS*/, UINT FirstSubresource /*= 0*/, UINT NumSubresources /*= D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES*/)
	{
		auto Resource = Uav->GetResource();
		if (Resource)
		{
			if (NumSubresources < D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES)
			{
				for (uint32_t i = 0; i < NumSubresources; ++i)
				{
					TransitionBarrier(Resource, StateAfter, FirstSubresource + i);
				}
			}
			else
			{
				TransitionBarrier(Resource, StateAfter);
			}

			TrackResource(Resource);
		}

		m_DynamicDescriptorHeap[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->StageDescriptors(RootParameterIndex, DescriptorOffset, 1, Uav->GetDescriptorHandle());

	}

	CD3DX12_GPU_DESCRIPTOR_HANDLE FDX12CommandList::AppendCbvSrvUavDescriptors(D3D12_CPU_DESCRIPTOR_HANDLE* DstDescriptor, uint32_t NumDescriptors)
	{
		return m_DescriptorCache->AppendCbvSrvUavDescriptors(DstDescriptor, NumDescriptors);
	}

	void FDX12CommandList::TrackResource(Microsoft::WRL::ComPtr<ID3D12Object> Object)
	{
		m_TrackedObjects.push_back(Object);
	}

	void FDX12CommandList::TrackResource(const Ref<FDX12Resource>& res)
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

	void FDX12CommandList::CopyBufferRegion(ComPtr<ID3D12Resource> DstResource, UINT64 DstOffset, ComPtr<ID3D12Resource> SrcResource, UINT64 SrcOffset, UINT64 Size)
	{
		m_D3DCommandList->CopyBufferRegion(DstResource.Get(), DstOffset, SrcResource.Get(), SrcOffset, Size);
	}

	void FDX12CommandList::CopyTextureRegion(const D3D12_TEXTURE_COPY_LOCATION* Dst, UINT DstX, UINT DstY, UINT DstZ, const D3D12_TEXTURE_COPY_LOCATION* Src, const D3D12_BOX* SrcBox)
	{
		m_D3DCommandList->CopyTextureRegion(Dst, DstX, DstY, DstZ, Src, SrcBox);
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

	void FDX12CommandList::CreateAndInitDefaultBuffer(D3D12_RESOURCE_DESC& ResourceDesc, const void* BufferData, uint32_t Size, uint32_t Alignment, FResourceLocation& ResourceLocation)
	{
		auto* DefaultBufferAllocator = FDX12Device::Get()->GetDefaultBufferAllocator();
		DefaultBufferAllocator->AllocDefaultResource(ResourceDesc, Alignment, ResourceLocation);

		if (BufferData != nullptr)
		{
			//Create upload resource 
			FResourceLocation UploadResourceLocation;
			auto UploadBufferAllocator = FDX12Device::Get()->GetUploadBufferAllocator();
			void* MappedData = UploadBufferAllocator->AllocUploadResource(Size, UPLOAD_RESOURCE_ALIGNMENT, UploadResourceLocation);

			memcpy(MappedData, BufferData, Size);

			Ref<FDX12Resource>	DefaultBuffer = ResourceLocation.m_UnderlyingResource;
			Ref<FDX12Resource>	UploadBuffer = UploadResourceLocation.m_UnderlyingResource;
			TransitionBarrier(DefaultBuffer->GetD3DResource().Get(), D3D12_RESOURCE_STATE_COPY_DEST);
			CopyBufferRegion(DefaultBuffer->GetD3DResource(), ResourceLocation.m_OffsetFromBaseOfResource,
				UploadBuffer->GetD3DResource(), UploadResourceLocation.m_OffsetFromBaseOfResource, Size);
			//UploadResourceLocation.ReleaseResource();
		}
	}

	void FDX12CommandList::CreateAndInitDefaultBuffer(const void* BufferData, uint32_t Size, ComPtr<ID3D12Resource> Resource)
	{
			auto upload_buffer = FDX12Device::Get()->GetLinearDynamicAllocator();
			FDynamicAllocation upload_alloc = upload_buffer->Allocate(Size);
			upload_alloc.Update(BufferData, Size);
			m_ResourceStateTracker->TransitionResource(Resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST);
			CopyBufferRegion(
				Resource,
				0,
				upload_alloc.UploadBuffer,
				upload_alloc.Offset,
				Size);
	}

	void FDX12CommandList::SetCompute32BitConstants(uint32_t RootParameterIndex, uint32_t NumConstants, const void* Constants)
	{
		m_D3DCommandList->SetComputeRoot32BitConstants(RootParameterIndex, NumConstants, Constants, 0);
	}

	void FDX12CommandList::SetGraphicsRootSignature(Ref<FDX12RootSignature> RootSignature)
	{
		auto* D3DRootSignature = RootSignature->GetD3D12RootSignature().Get();
		if (D3DRootSignature != m_RootSignature)
		{
			m_RootSignature = D3DRootSignature;
			m_D3DCommandList->SetGraphicsRootSignature(m_RootSignature);
			TrackResource(m_RootSignature);
		}
	}

	void FDX12CommandList::SetComputeRootSignature(Ref<FDX12RootSignature> RootSignature)
	{
		auto* D3DRootSignature = RootSignature->GetD3D12RootSignature().Get();
		if (D3DRootSignature != m_RootSignature)
		{
			m_RootSignature = D3DRootSignature;
			m_D3DCommandList->SetComputeRootSignature(m_RootSignature);
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
			m_DynamicDescriptorHeap[i]->Reset();
			m_DescriptorHeaps[i] = nullptr;
		}

		m_RootSignature = nullptr;
		m_PipelineState = nullptr;
		m_ComputeCommandList = nullptr;
		m_DescriptorCache->Reset();
	}


	void FDX12CommandList::ReleaseTrackedObjects()
	{
		m_TrackedObjects.clear();
	}


	void FDX12CommandList::Execute()
	{
	}
}
