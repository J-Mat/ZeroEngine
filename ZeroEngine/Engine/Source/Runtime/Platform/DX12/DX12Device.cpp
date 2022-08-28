#include "DX12CommandQueue.h"
#include "DX12Device.h"
#include "Platform/DX12/DX12SwapChain.h"
#include "MemoryManage/DescriptorAllocator.h"


namespace Zero
{
	void FDX12Device::Init()
	{
		EnableDebugLayer();
		CreateDevice();
		GetDescriptorSize();
		CreateCommandQueue();
		CreateDescriptors();

		CheckFeatures();
		
		
	}
	
	FDescriptorAllocation FDX12Device::AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE Type, uint32_t NumDescriptors)
	{
		return m_DescriptorAllocators[Type]->Allocate(NumDescriptors);
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

	void FDX12Device::CreatSwapChain(HWND hWnd)
	{
		m_SwapChain = CreateRef<FDX12SwapChain>(*this, hWnd);
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
		m_DirectCommandQueue = CreateScope<FDX12CommandQueue>(*this, D3D12_COMMAND_LIST_TYPE_DIRECT);
		m_ComputeCommandQueue = CreateScope<FDX12CommandQueue>(*this, D3D12_COMMAND_LIST_TYPE_COMPUTE);
		m_CopyCommandQueue = CreateScope<FDX12CommandQueue>(*this, D3D12_COMMAND_LIST_TYPE_COPY);
	}

	void FDX12Device::CreateDescriptors()
	{
		for (int i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i)
		{
			m_DescriptorAllocators[i] = CreateRef<FDescriptorAllocator>(*this, static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>(i));
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
}