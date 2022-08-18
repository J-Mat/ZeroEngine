#include "DX12Device.h"


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
		
		ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&m_DxgiFactory)));

		HRESULT  HardwardResult = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_D3DDevice));
		if (FAILED(HardwardResult))
		{
			ComPtr<IDXGIAdapter> WarpAdapterPtr;
			ThrowIfFailed(m_DxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&WarpAdapterPtr)));

			ThrowIfFailed(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_D3DDevice)));
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
			m_DescriptorAllocators[i] = CreateScope<FDescriptorAllocator>(this, static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>(i));
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
}