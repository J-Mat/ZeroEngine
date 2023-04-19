#include "DX12CommandQueue.h"
#include "DX12Device.h"
#include "Platform/DX12/DX12SwapChain.h"
#include "MemoryManage/Descriptor/DescriptorAllocator.h"
#include "DX12CommandList.h"
#include "DX12Texture2D.h"
#include "DX12TextureCube.h"
#include "DX12Mesh.h"
#include "./Shader/DX12Shader.h"
#include "./PSO/DX12PipelineStateObject.h"
#include "Core/Framework/Library.h"
#include "Render/Moudule/MeshGenerator.h"
#include "./Shader/DX12ShaderCompiler.h"
#include "DX12RenderTargetCube.h"
#include "ZConfig.h"


namespace Zero
{
	FDX12Device* FDX12Device::m_Instance = nullptr;
	FDX12Device::FDX12Device()
	{
		CORE_ASSERT(m_Instance == nullptr, "Device must be the only one!")
		if (m_Instance == nullptr)
		{
			m_Instance = this;
		}
	}
	void FDX12Device::Init()
	{
		EnableDebugLayer();
		CreateDevice();
		GetDescriptorSize();
		CreateCommandQueue();
		CreateDescriptors();
		CheckFeatures();
		CreateGuiDescHeap();

		D3D12MA::ALLOCATOR_DESC allocator_desc{};
		allocator_desc.pDevice = m_D3DDevice.Get();
		allocator_desc.pAdapter = m_Adapter->GetDXGIAdapter().Get();
		D3D12MA::Allocator* _allocator = nullptr;
		D3D12MA::CreateAllocator(&allocator_desc, &_allocator);
		m_MemAllocator.reset(_allocator);

		m_DefaultBufferAllocator = CreateScope<FDefaultBufferAllocator>();
		m_UploadBufferAllocator = CreateScope<FUploadBufferAllocator>();
		m_TextureResourceAllocator = CreateScope<FTextureResourceAllocator>();

		for (int i = 0; i < IDevice::BACKBUFFER_COUNT; ++i)
		{
			m_DynamicAllocators.push_back(CreateScope<FLinearDynamicAllocator>(50000000));  // 50'000'000
		}

		PreInitWorld();

		FDX12ShaderCompiler::Get().Init();
	}
	
	FDescriptorAllocation FDX12Device::AllocateRuntimeDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE Type, uint32_t NumDescriptors)
	{
		return m_DescriptorAllocators[Type]->Allocate(NumDescriptors);
	}

	void FDX12Device::CreateGuiDescHeap(uint32_t NumDescriptors)
	{
		D3D12_DESCRIPTOR_HEAP_DESC Desc = {};
		Desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		Desc.NumDescriptors = NumDescriptors;
		Desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		ThrowIfFailed(m_D3DDevice->CreateDescriptorHeap(&Desc, IID_PPV_ARGS(&m_GUISrvDescHeap)));
	}

	FLightDescrptorAllocation FDX12Device::AllocateGuiDescritor()
	{
		D3D12_CPU_DESCRIPTOR_HANDLE CpuHandle = m_GUISrvDescHeap->GetCPUDescriptorHandleForHeapStart();
		CpuHandle.ptr += m_Cbv_Srv_UavDescriptorSize * m_CurGuiDescHeapIndex;
		D3D12_GPU_DESCRIPTOR_HANDLE GpuHandle = m_GUISrvDescHeap->GetGPUDescriptorHandleForHeapStart();
		GpuHandle.ptr += m_Cbv_Srv_UavDescriptorSize * m_CurGuiDescHeapIndex;
		m_CurGuiDescHeapIndex++;
		return { CpuHandle, GpuHandle };
	}

	void FDX12Device::ReleaseStaleDescriptors()
	{
		for (int i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i)
		{
			m_DescriptorAllocators[i]->ReleaseStaleDescriptors();
		}
	}

	void FDX12Device::Flush()
	{
		m_CommandQueue[ERenderPassType::Graphics]->Flush();
		m_CommandQueue[ERenderPassType::Compute]->Flush();
		m_CommandQueue[ERenderPassType::Copy]->Flush();
	}

	void FDX12Device::CreateSwapChain(HWND hWnd)
	{
		m_SwapChain = CreateRef<FDX12SwapChain>(hWnd);
		m_SwapChain->SetVSync(false);
	}

	FLinearDynamicAllocator* FDX12Device::GetLinearDynamicAllocator()
	{
		return m_DynamicAllocators[m_SwapChain->GetCurrentBufferIndex()].get();
	}

	D3D12MA::Allocator* FDX12Device::GetMemAllocator() const
	{
		return m_MemAllocator.get();
	}

	Scope<FPipelineStateObject> FDX12Device::CreatePSO(const FPSODescriptor& PSODescriptor)
	{
		return CreateScope<FDX12PipelineStateObject>(PSODescriptor);
	}

	FTexture2D* FDX12Device::CreateTexture2D(const std::string& TextureName, const FTextureDesc& Desc, bool bCreateTextureView /*= true*/)
	{
		return new FDX12Texture2D(TextureName, Desc, bCreateTextureView);
	}

	Ref<FTexture2D> FDX12Device::GetOrCreateTexture2D(const std::string& Filename, bool bNeedMipMap)
	{
		std::filesystem::path TextureFileName = Filename;
		Ref<FTexture2D> Texture{};
		std::filesystem::path TexturePath = ZConfig::GetAssestsFullPath(Filename);
		if (std::filesystem::exists(TexturePath))
		{
			Ref<FImage> Image = CreateRef<FImage>(TexturePath.string());
			Texture = CreateRef<FDX12Texture2D>(Filename, Image, bNeedMipMap);
		}
		return Texture;
	
	}	
	Ref<FShader> FDX12Device::CreateShader(const FShaderBinderDesc& BinderDesc, const FShaderDesc& ShaderDesc)
	{
		Ref<FShader> Shader = TLibrary<FShader>::Fetch(ShaderDesc.FileName);
		if (Shader == nullptr)
		{
			Shader = CreateRef<FDX12Shader>(BinderDesc, ShaderDesc);
			TLibrary<FShader>::Push(ShaderDesc.FileName, Shader);
		}
		return Shader;
	}

	Ref<FShader> FDX12Device::CreateShader(const FShaderDesc& ShaderDesc)
	{
		return CreateRef<FDX12Shader>(ShaderDesc);
	}

	Ref<FMesh> FDX12Device::CreateMesh(const std::vector<FMeshData>& MeshDatas, FVertexBufferLayout& Layout)
	{
		return CreateRef<FDX12Mesh>(MeshDatas, Layout);
	}

	Ref<FMesh> FDX12Device::CreateMesh(float* Vertices, uint32_t VertexCount, uint32_t* Indices, uint32_t IndexCount, FVertexBufferLayout& Layout)
	{
		return CreateRef<FDX12Mesh>(Vertices, VertexCount, Indices, IndexCount, Layout);
	}

	FBuffer* FDX12Device::CreateBuffer(const FBufferDesc& Desc)
	{
		return new FDX12Buffer(Desc);
	}

	void FDX12Device::BindVertexBuffer(FCommandListHandle Handle, FBuffer* VertexBuffer)
	{
		D3D12_VERTEX_BUFFER_VIEW VBView{};
		VBView.BufferLocation = static_cast<FDX12Buffer*>(VertexBuffer)->GetGPUAddress();
		VBView.SizeInBytes = static_cast<UINT>(VertexBuffer->GetDesc().Size);
		VBView.StrideInBytes = static_cast<UINT>(VertexBuffer->GetDesc().Stride);
		GetCommandList(Handle)->GetD3D12CommandList()->IASetVertexBuffers(0, 1, &VBView);
	}

	void FDX12Device::BindIndexBuffer(FCommandListHandle Handle, FBuffer* IndexBuffer)
	{
		D3D12_INDEX_BUFFER_VIEW ib_view{};
		ib_view.BufferLocation = static_cast<FDX12Buffer*>(IndexBuffer)->GetGPUAddress();
		ib_view.Format = FDX12Utils::ConvertResourceFormat(IndexBuffer->GetDesc().Format);
		ib_view.SizeInBytes = IndexBuffer->GetDesc().Size;
		GetCommandList(Handle)->GetD3D12CommandList()->IASetIndexBuffer(&ib_view);
	}

	Ref<FTextureCube> FDX12Device::GetOrCreateTextureCubemap(FTextureHandle Handles[CUBEMAP_TEXTURE_CNT], std::string TextureCubemapName)
	{
		Ref<FTextureCube> TextureCubemap;
		Ref<FImage> ImageData[CUBEMAP_TEXTURE_CNT]	;
		for (int i = 0; i < CUBEMAP_TEXTURE_CNT; ++i)
		{
			Ref<FTexture2D> Texture2D = GetOrCreateTexture2D(Handles[i].TextureName);
			ImageData[i] = Texture2D->GetImage();
		}
		TextureCubemap = CreateRef<FDX12TextureCube>(TextureCubemapName, ImageData);
		return TextureCubemap;
	}

	Ref<FRenderTarget2D> FDX12Device::CreateRenderTarget2D(const FRenderTarget2DDesc& Desc)
	{
		return CreateRef<FDX12RenderTarget2D>(Desc);
	}

	FRenderTarget2D* FDX12Device::CreateRenderTarget2D()
	{
		return new FDX12RenderTarget2D();
	}

	Ref<FRenderTargetCube> FDX12Device::CreateRenderTargetCube(const FRenderTargetCubeDesc& Desc)
	{
		return CreateRef<FDX12RenderTargetCube>(Desc);
	}

	void FDX12Device::EnableDebugLayer()
	{
		#if defined(DEBUG) || defined(_DEBUG)
		{
			ComPtr<ID3D12Debug> DebugController;
			ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&DebugController)));
			DebugController->EnableDebugLayer();
		}
		#endif	
	}

	
	void FDX12Device::CreateDevice()
	{
		m_Adapter = FAdapter::Create();
		
		auto DxgiAdapter = m_Adapter->GetDXGIAdapter();
		ThrowIfFailed(D3D12CreateDevice(DxgiAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_D3DDevice)));
		
		// Enable debug messages (only works if the debug layer has already been enabled).
		ComPtr<ID3D12InfoQueue> pInfoQueue;
		if (SUCCEEDED(m_D3DDevice.As(&pInfoQueue)))
		{
			pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
			pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
			pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);

			// Suppress whole categories of messages
			// D3D12_MESSAGE_CATEGORY Categories[] = {};

			// Suppress messages based on their severity level
			D3D12_MESSAGE_SEVERITY Severities[] = { D3D12_MESSAGE_SEVERITY_INFO };

			// Suppress individual messages by their ID
			D3D12_MESSAGE_ID DenyIds[] = {
				D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,  // I'm really not sure how to avoid this
																			   // message.

				D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,  // This warning occurs when using capture frame while graphics
														 // debugging.

				D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,  // This warning occurs when using capture frame while graphics

														   // debugging.

	// Workarounds for debug layer issues on hybrid-graphics systems
				//D3D12_MESSAGE_ID_EXECUTECOMMANDLISTS_WRONGSWAPCHAINBUFFERREFERENCE,
				D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE,
				//D3D12_MESSAGE_ID_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT_INVALIDOUTPUTSLOT
			};

			D3D12_INFO_QUEUE_FILTER NewFilter = {};
			// NewFilter.DenyList.NumCategories = _countof(Categories);
			// NewFilter.DenyList.pCategoryList = Categories;
			NewFilter.DenyList.NumSeverities = _countof(Severities);
			NewFilter.DenyList.pSeverityList = Severities;
			NewFilter.DenyList.NumIDs = _countof(DenyIds);
			NewFilter.DenyList.pIDList = DenyIds;

			ThrowIfFailed(pInfoQueue->PushStorageFilter(&NewFilter));
		}
	}
	
	void FDX12Device::GetDescriptorSize()
	{
		m_RtvDescriptorSize = m_D3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		m_DsvDescriptorSize = m_D3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		m_Cbv_Srv_UavDescriptorSize = m_D3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}
	
	void FDX12Device::CreateCommandQueue()
	{
		m_CommandQueue.resize(ERenderPassType::TypeNum);

		m_CommandQueue[ERenderPassType::Graphics] = CreateScope<FDX12CommandQueue>(D3D12_COMMAND_LIST_TYPE_DIRECT);
		m_CommandLists.push_back({});

		m_CommandQueue[ERenderPassType::Compute] = CreateScope<FDX12CommandQueue>(D3D12_COMMAND_LIST_TYPE_COMPUTE);
		m_CommandLists.push_back({});

		m_CommandQueue[ERenderPassType::Copy] = CreateScope<FDX12CommandQueue>(D3D12_COMMAND_LIST_TYPE_COPY);
		m_CommandLists.push_back({});
		
	}

	void FDX12Device::CreateDescriptors()
	{
		for (int i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i)
		{
			m_DescriptorAllocators[i] = CreateRef<FDescriptorAllocator>(static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>(i));
		}
	}

	void FDX12Device::CheckFeatures()
	{
		D3D12_FEATURE_DATA_ROOT_SIGNATURE FeatureData;
		FeatureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
		if (FAILED(m_D3DDevice->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &FeatureData,
			sizeof(D3D12_FEATURE_DATA_ROOT_SIGNATURE))))
		{
			FeatureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
		}
		m_HighestRootSignatureVersion = FeatureData.HighestVersion;
	}

	FDX12CommandQueue& FDX12Device::GetCommandQueue(ERenderPassType Type)
	{
		return *m_CommandQueue[Type].get();
	}

	std::wstring FDX12Device::GetDescription() const
	{
		return m_Adapter->GetDescription();
	}

	std::vector<CD3DX12_STATIC_SAMPLER_DESC> FDX12Device::GetStaticSamplers(std::vector<EShaderSampler>& ShaderSamplers)
	{
		//过滤器POINT,寻址模式WRAP的静态采样器
		static CD3DX12_STATIC_SAMPLER_DESC PointWarp(0,	//着色器寄存器
			D3D12_FILTER_MIN_MAG_MIP_POINT,		//过滤器类型为POINT(常量插值)
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,	//U方向上的寻址模式为WRAP（重复寻址模式）
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,	//V方向上的寻址模式为WRAP（重复寻址模式）
			D3D12_TEXTURE_ADDRESS_MODE_WRAP);	//W方向上的寻址模式为WRAP（重复寻址模式）

		//过滤器POINT,寻址模式CLAMP的静态采样器
		static CD3DX12_STATIC_SAMPLER_DESC PointClamp(1,	//着色器寄存器
			D3D12_FILTER_MIN_MAG_MIP_POINT,		//过滤器类型为POINT(常量插值)
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,	//U方向上的寻址模式为CLAMP（钳位寻址模式）
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,	//V方向上的寻址模式为CLAMP（钳位寻址模式）
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP);	//W方向上的寻址模式为CLAMP（钳位寻址模式）

		//过滤器LINEAR,寻址模式WRAP的静态采样器
		static CD3DX12_STATIC_SAMPLER_DESC LinearWarp(2,	//着色器寄存器
			D3D12_FILTER_MIN_MAG_MIP_LINEAR,		//过滤器类型为LINEAR(线性插值)
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,	//U方向上的寻址模式为WRAP（重复寻址模式）
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,	//V方向上的寻址模式为WRAP（重复寻址模式）
			D3D12_TEXTURE_ADDRESS_MODE_WRAP);	//W方向上的寻址模式为WRAP（重复寻址模式）

		//过滤器LINEAR,寻址模式CLAMP的静态采样器
		static CD3DX12_STATIC_SAMPLER_DESC LinearClamp(3,	//着色器寄存器
			D3D12_FILTER_MIN_MAG_MIP_LINEAR,		//过滤器类型为LINEAR(线性插值)
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,	//U方向上的寻址模式为CLAMP（钳位寻址模式）
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,	//V方向上的寻址模式为CLAMP（钳位寻址模式）
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP);	//W方向上的寻址模式为CLAMP（钳位寻址模式）

		//过滤器ANISOTROPIC,寻址模式WRAP的静态采样器
		static CD3DX12_STATIC_SAMPLER_DESC AnisotropicWarp(4,	//着色器寄存器
			D3D12_FILTER_ANISOTROPIC,			//过滤器类型为ANISOTROPIC(各向异性)
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,	//U方向上的寻址模式为WRAP（重复寻址模式）
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,	//V方向上的寻址模式为WRAP（重复寻址模式）
			D3D12_TEXTURE_ADDRESS_MODE_WRAP);	//W方向上的寻址模式为WRAP（重复寻址模式）

		//过滤器LINEAR,寻址模式CLAMP的静态采样器
		static CD3DX12_STATIC_SAMPLER_DESC AnisotropicClamp(5,	//着色器寄存器
			D3D12_FILTER_ANISOTROPIC,			//过滤器类型为ANISOTROPIC(各向异性)
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,	//U方向上的寻址模式为CLAMP（钳位寻址模式）
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,	//V方向上的寻址模式为CLAMP（钳位寻址模式）
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP);	//W方向上的寻址模式为CLAMP（钳位寻址模式）


		static CD3DX12_STATIC_SAMPLER_DESC Shadow(
			6, // shaderRegister
			D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, // filter
			D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressW
			0.0f,                               // mipLODBias
			16,                                 // maxAnisotropy
			D3D12_COMPARISON_FUNC_LESS_EQUAL,
			D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK);

		std::vector<CD3DX12_STATIC_SAMPLER_DESC> Res;
		for (EShaderSampler ShaderSamplerType : ShaderSamplers)
		{
			switch (ShaderSamplerType)
			{
			case Zero::EShaderSampler::PointWarp:
				Res.push_back(PointWarp);
				break;
			case Zero::EShaderSampler::PointClamp:
				Res.push_back(PointClamp);
				break;
			case Zero::EShaderSampler::LinearWarp:
				Res.push_back(LinearWarp);
				break;
			case Zero::EShaderSampler::LinearClamp:
				Res.push_back(LinearClamp);
				break;
			case Zero::EShaderSampler::AnisotropicWarp:
				Res.push_back(AnisotropicWarp);
				break;
			case Zero::EShaderSampler::AnisotropicClamp:
				Res.push_back(AnisotropicClamp);
				break;
			case Zero::EShaderSampler::Shadow:
				Res.push_back(Shadow);
				break;
			default:
				break;
			}
		}
		return Res;
	}


	FCommandListHandle FDX12Device::GenerateCommanList(ERenderPassType RenderPassType)
	{
		auto CommanList = m_CommandQueue[uint32_t(RenderPassType)]->GetCommandList();
		m_CommandLists[RenderPassType].push_back(CommanList);
		auto Handle = FCommandListHandle(m_CommandLists[RenderPassType].size() - 1);
		CommanList->SetComandListHandle(Handle);
		return Handle;
	}

	Ref<FDX12CommandList> FDX12Device::GetCommandList(FCommandListHandle Handle, ERenderPassType RenderPassType)
	{
		return m_CommandLists[uint32_t(RenderPassType)][Handle];
	}

	Ref<FCommandList> FDX12Device::GetRHICommandList(FCommandListHandle Handle, ERenderPassType RenderPassType /*= ERenderPassType::Graphics*/)
	{
		return m_CommandLists[uint32_t(RenderPassType)][Handle];
	}

	void FDX12Device::PreInitWorld()
	{
		FCommandListHandle InitWorldCommandListHandle = GenerateCommanList(ERenderPassType::Graphics);
		FDX12Device::Get()->SetInitWorldCommandList(InitWorldCommandListHandle);
	}

	void FDX12Device::FlushInitCommandList()
	{
		m_CommandQueue[ERenderPassType::Graphics]->ExecuteCommandList(GetCommandList(m_InitWorldCommandListHandle));
	}

	uint64_t FDX12Device::GetCurrentFenceValue()
	{
		return m_SwapChain->GetCurrentFenceValue();
	}

	void FDX12Device::ClearBackBuffer()
	{
		uint32_t CurrentBackIndex = m_SwapChain->GetCurrentBufferIndex();
		m_DynamicAllocators[CurrentBackIndex]->CleanUpAllocations();
	}

	void FDX12Device::BeginFrame()
	{
		m_CommandLists[ERenderPassType::Graphics].clear();
		m_CommandLists[ERenderPassType::Compute].clear();
		m_CommandLists[ERenderPassType::Copy].clear();

		ClearBackBuffer();
		InitSingleThreadCommandLists();
	}

	void FDX12Device::InitSingleThreadCommandLists()
	{
		m_SingleThreadCommandListHandles[ERenderPassType::Graphics] = GenerateCommanList(ERenderPassType::Graphics);
		m_SingleThreadCommandListHandles[ERenderPassType::Compute] = GenerateCommanList(ERenderPassType::Compute);
		m_SingleThreadCommandListHandles[ERenderPassType::Copy] = GenerateCommanList(ERenderPassType::Copy);
	}

	void FDX12Device::ExecuteSingleThreadCommandLists()
	{
		{
			auto ComandList = GetCommandList(m_SingleThreadCommandListHandles[ERenderPassType::Graphics], ERenderPassType::Graphics);
			GetCommandQueue(ERenderPassType::Graphics).ExecuteCommandList(ComandList);
		}
		{
			auto ComandList = GetCommandList(m_SingleThreadCommandListHandles[ERenderPassType::Compute], ERenderPassType::Compute);
			GetCommandQueue(ERenderPassType::Compute).ExecuteCommandList(ComandList);
		}
		{
			auto ComandList = GetCommandList(m_SingleThreadCommandListHandles[ERenderPassType::Copy], ERenderPassType::Copy);
			GetCommandQueue(ERenderPassType::Copy).ExecuteCommandList(ComandList);
		}
	}

	void FDX12Device::EndFrame()
	{
		m_DefaultBufferAllocator->CleanUpAllocations();
		m_UploadBufferAllocator->CleanUpAllocations();
		m_TextureResourceAllocator->CleanUpAllocations();
		FResourceStateTracker::RemoveGarbageResources();
	}
	
	FDX12Texture2D* FDX12Device::CreateDepthTexture(const std::string& TextureName, uint32_t Width, uint32_t Height)
	{
		D3D12_CLEAR_VALUE OptClear = {};
		OptClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		OptClear.DepthStencil = { 1.0F, 0 };
		FTextureClearValue ClearValue(1.0f, 0);
		FTextureDesc Desc = {
			.Width = Width,
			.Height = Height,
			.ResourceBindFlags = EResourceBindFlag::DepthStencil | EResourceBindFlag::ShaderResource,
			.ClearValue = ClearValue,
			.Format = EResourceFormat::D24_UNORM_S8_UINT,
		};
		return new FDX12Texture2D(TextureName, Desc, true);
	}

	uint32_t FDX12Device::RegisterActiveComandlist(Ref<FDX12CommandList> CommandList)
	{
		m_ActiveCommandListMap.insert({ m_CommandListID , CommandList });
		return m_CommandListID++;
	}
	void FDX12Device::UnRegisterActiveComandlist(uint32_t ID)
	{
		m_ActiveCommandListMap.erase(ID);
	}
	Ref<FDX12CommandList> FDX12Device::GetActiveCommandList(uint32_t Slot)
	{
		auto Iter = m_ActiveCommandListMap.find(Slot);
		CORE_ASSERT(Iter != m_ActiveCommandListMap.end(), "CommandList dose not exits !");
		return Iter->second;
	}
}