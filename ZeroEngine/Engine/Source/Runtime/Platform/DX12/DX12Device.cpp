#include "DX12CommandQueue.h"
#include "DX12Device.h"
#include "Platform/DX12/DX12SwapChain.h"
#include "MemoryManage/DescriptorAllocator.h"
#include "DX12CommandList.h"

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
		m_DirectCommandQueue->Flush();
		m_ComputeCommandQueue->Flush();
		m_CopyCommandQueue->Flush();
	}

	void FDX12Device::CreateSwapChain(HWND hWnd)
	{
		m_SwapChain = CreateRef<FDX12SwapChain>(hWnd);
		m_SwapChain->SetVSync(false);
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
		m_DirectCommandQueue = CreateScope<FDX12CommandQueue>(D3D12_COMMAND_LIST_TYPE_DIRECT);
		m_ComputeCommandQueue = CreateScope<FDX12CommandQueue>(D3D12_COMMAND_LIST_TYPE_COMPUTE);
		m_CopyCommandQueue = CreateScope<FDX12CommandQueue>(D3D12_COMMAND_LIST_TYPE_COPY);
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

	FDX12CommandQueue& FDX12Device::GetCommandQueue(D3D12_COMMAND_LIST_TYPE Type)
	{
		FDX12CommandQueue* CommandQueue;
		switch (Type)
		{
		case D3D12_COMMAND_LIST_TYPE_DIRECT:
			CommandQueue = m_DirectCommandQueue.get();
			break;
		case D3D12_COMMAND_LIST_TYPE_COMPUTE:
			CommandQueue = m_ComputeCommandQueue.get();
			break;
		case D3D12_COMMAND_LIST_TYPE_COPY:
			CommandQueue = m_CopyCommandQueue.get();
			break;
		default:
			CORE_ASSERT(false, "Invalid command queue type.");
		}

		return *CommandQueue;
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

	void FDX12Device::PreInitWorld()
	{
		SetInitWorldCommandList(m_DirectCommandQueue->GetCommandList());
	}

	void FDX12Device::FlushInitCommandList()
	{
		m_DirectCommandQueue->ExecuteCommandList(m_InitWorldCommandList);
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