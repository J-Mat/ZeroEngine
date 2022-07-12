#include "GPUWaves.h"

FGPUWaves::FGPUWaves(ID3D12Device* Device, ID3D12GraphicsCommandList* CmdList, int m, int n, float dx, float dt, float Speed, float Damping)
{
	D3DDevice = Device;

	NumRows = m;
	NumCols = n;

	assert((m * n) % 256 == 0);

	VertexCount = m * n;
	TriangleCount = (m - 1) * (n - 1) * 2;

	TimeStep = dt;
	SpatialStep = dx;

	float d = Damping * dt + 2.0f;
	float e = (Speed * Speed) * (dt * dt) / (dx * dx);
	K[0] = (Damping * dt - 2.0f) / d;
	K[1] = (4.0f - 8.0f * e) / d;
	K[2] = (2.0f * e) / d;

	BuildResources(CmdList);
}

UINT FGPUWaves::RowCount() const
{
	return NumRows;
}

UINT FGPUWaves::ColumnCount() const
{
	return NumCols;
}

UINT FGPUWaves::GetVertexCount() const
{
	return VertexCount;
}

UINT FGPUWaves::GetTriangleCount() const
{
	return TriangleCount;
}

float FGPUWaves::Width() const
{
	return NumCols * SpatialStep;
}

float FGPUWaves::Depth() const
{
	return NumRows * SpatialStep;
}

float FGPUWaves::GetSpatialStep() const
{
	return SpatialStep;
}


UINT FGPUWaves::DescriptorCount() const
{
	return 6;
}

void FGPUWaves::BuildResources(ID3D12GraphicsCommandList* CmdList)
{
	// All the textures for the wave simulation will be bound as a shader resource and
	// unordered access view at some point since we ping-pong the buffers.
	
	D3D12_RESOURCE_DESC TexDesc;
	ZeroMemory(&TexDesc, sizeof(D3D12_RESOURCE_DESC));
	TexDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	TexDesc.Alignment = 0;
	TexDesc.Width = NumCols;
	TexDesc.Height = NumRows;
	TexDesc.DepthOrArraySize = 1;
	TexDesc.MipLevels = 1;
	TexDesc.Format = DXGI_FORMAT_R32_FLOAT;
	TexDesc.SampleDesc.Count = 1;
	TexDesc.SampleDesc.Quality = 0;
	TexDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	TexDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	
	ThrowIfFailed(
		D3DDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&TexDesc,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(&PrevSol)
		)
	);

	ThrowIfFailed(
		D3DDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&TexDesc,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(&CurrSol)
		)
	);
	
	ThrowIfFailed(
		D3DDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&TexDesc,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(&NextSol)
		)
	);

	//
	// In order to copy CPU memory data into our default buffer, we need to create
	// an intermediate upload heap. 
	//
	
	const UINT Num2DSubresources = TexDesc.DepthOrArraySize * TexDesc.MipLevels;
	const UINT64 UploadBufferSize = GetRequiredIntermediateSize(CurrSol.Get(), 0, Num2DSubresources);
	
	ThrowIfFailed(
		D3DDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(UploadBufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(PrevUploadBuffer.GetAddressOf())
		)
	);
	
	ThrowIfFailed(
		D3DDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(UploadBufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(CurrUploadBuffer.GetAddressOf())
		)
	);
	
	// Describe the data we want to copy into the default buffer.
	std::vector<float> InitData(NumCols * NumRows, 0.0f);
	for (int i = 0; i < InitData.size(); ++i)
	{
		InitData[i] = 0.0f;
	}

	D3D12_SUBRESOURCE_DATA SubSeoruceData = {};
	SubSeoruceData.pData = InitData.data();
	SubSeoruceData.RowPitch = NumCols * sizeof(float);
	SubSeoruceData.SlicePitch = SubSeoruceData.RowPitch * NumRows;
	
	//
	// Schedule to copy the data to the default resource, and change states.
	// Note that mCurrSol is put in the GENERIC_READ state so it can be 
	// read by a shader.
	//
	CmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(PrevSol.Get(),
		D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));
	UpdateSubresources(CmdList, PrevSol.Get(), PrevUploadBuffer.Get(), 0, 0, Num2DSubresources, &SubSeoruceData);
	CmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(PrevSol.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_UNORDERED_ACCESS));

	CmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrSol.Get(),
		D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));
	UpdateSubresources(CmdList, CurrSol.Get(), CurrUploadBuffer.Get(), 0, 0, Num2DSubresources, &SubSeoruceData);
	CmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrSol.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));

	CmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(NextSol.Get(),
		D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS));
}

void FGPUWaves::BuildDescriptors(CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuDescriptor, CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuDescriptor, UINT DescriptorSize)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC SrvDesc = {};
	SrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	SrvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	SrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	SrvDesc.Texture2D.MostDetailedMip = 0;
	SrvDesc.Texture2D.MipLevels = 1;
	
	D3D12_UNORDERED_ACCESS_VIEW_DESC UavDesc = {};

	UavDesc.Format = DXGI_FORMAT_R32_FLOAT;
	UavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	UavDesc.Texture2D.MipSlice = 0;

	D3DDevice->CreateShaderResourceView(PrevSol.Get(), &SrvDesc, hCpuDescriptor);
	D3DDevice->CreateShaderResourceView(CurrSol.Get(), &SrvDesc, hCpuDescriptor.Offset(1, DescriptorSize));
	D3DDevice->CreateShaderResourceView(NextSol.Get(), &SrvDesc, hCpuDescriptor.Offset(1, DescriptorSize));
	
	D3DDevice->CreateUnorderedAccessView(PrevSol.Get(), nullptr, &UavDesc, hCpuDescriptor.Offset(1, DescriptorSize));
	D3DDevice->CreateUnorderedAccessView(CurrSol.Get(), nullptr, &UavDesc, hCpuDescriptor.Offset(1, DescriptorSize));
	D3DDevice->CreateUnorderedAccessView(NextSol.Get(), nullptr, &UavDesc, hCpuDescriptor.Offset(1, DescriptorSize));

	// Save references to the GPU descriptors. 
	PrevSolSrv = hGpuDescriptor;
	CurrSolSrv = hGpuDescriptor.Offset(1, DescriptorSize);
	NextSolSrv = hGpuDescriptor.Offset(1, DescriptorSize);
	PrevSolUav = hGpuDescriptor.Offset(1, DescriptorSize);
	CurrSolUav = hGpuDescriptor.Offset(1, DescriptorSize);
	NextSolUav = hGpuDescriptor.Offset(1, DescriptorSize);
}


void FGPUWaves::Update(const GameTimer& gt, ID3D12GraphicsCommandList* CmdList, ID3D12RootSignature* RootSig, ID3D12PipelineState* PSO)
{
	static float t = 0.0f;
	
	t += gt.DeltaTime();
	
	CmdList->SetPipelineState(PSO);
	CmdList->SetComputeRootSignature(RootSig);
	
	if (t >= TimeStep)
	{
		// Set the update constants.
		CmdList->SetComputeRoot32BitConstants(0, 3, K, 0);

		CmdList->SetComputeRootDescriptorTable(1, PrevSolUav);
		CmdList->SetComputeRootDescriptorTable(2, CurrSolUav);
		CmdList->SetComputeRootDescriptorTable(3, NextSolUav);

		// How many groups do we need to dispatch to cover the wave grid.  
		// Note that mNumRows and mNumCols should be divisible by 16
		// so there is no remainder.	

		UINT NumGroupsX = NumCols / 16;
		UINT NumGroupsY = NumRows / 16;
		CmdList->Dispatch(NumGroupsX, NumGroupsY, 1);

		//
		// Ping-pong buffers in preparation for the next update.
		// The previous solution is no longer needed and becomes the target of the next solution in the next update.
		// The current solution becomes the previous solution.
		// The next solution becomes the current solution.
		//

		auto ResTemp = PrevSol;
		PrevSol = CurrSol;
		CurrSol = NextSol;
		NextSol = ResTemp;

		auto SrvTemp = PrevSolSrv;
		PrevSolSrv = CurrSolSrv;
		CurrSolSrv = NextSolSrv;
		NextSolSrv = SrvTemp;

		auto UavTemp = PrevSolUav;
		PrevSolUav = CurrSolUav;
		CurrSolUav = NextSolUav;
		NextSolUav = UavTemp;
		
		// The current solution needs to be able to be read by the vertex shader, so change its state to GENERIC_READ.
		CmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrSol.Get(),
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_GENERIC_READ));
	}
	
	
}

void FGPUWaves::Disturb(ID3D12GraphicsCommandList* CmdList, ID3D12RootSignature* RootSig, ID3D12PipelineState* PSO, UINT i, UINT j, float Magnitude)
{
	CmdList->SetPipelineState(PSO);
	CmdList->SetComputeRootSignature(RootSig);
	
	// Set the disturb constants.
	UINT DisturbIndex[2] = { j, i };
	CmdList->SetComputeRoot32BitConstants(0, 1, &Magnitude, 3);
	CmdList->SetComputeRoot32BitConstants(0, 2, DisturbIndex, 4);
	
	CmdList->SetComputeRootDescriptorTable(3, CurrSolUav);

	// The current solution is in the GENERIC_READ state so it can be read by the vertex shader.
	// Change it to UNORDERED_ACCESS for the compute shader.  Note that a UAV can still be
	// read in a compute shader.
	CmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrSol.Get(),
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS));

	// One thread group kicks off one thread, which displaces the height of one
	// vertex and its neighbors.
	CmdList->Dispatch(1, 1, 1);
}

