#include "BlurFilter.h"

FBlurFilter::FBlurFilter(ID3D12Device* InDevice, UINT InWidth, UINT InHeight, DXGI_FORMAT InFormat) :
Device(InDevice),
Width(InWidth),
Height(InHeight),
Format(InFormat)
{
	BuildResources();
}

ID3D12Resource* FBlurFilter::Output()
{
	return BlurMap0.Get();
}

std::vector<float> FBlurFilter::CalcGaussWeights(float Sigma)
{
	float TwoSigma2 = 2.0f * Sigma * Sigma;

	// Estimate the blur radius based on sigma since sigma controls the "width" of the bell curve.
	// For example, for sigma = 3, the width of the bell curve is 
	int BlurRadius = (int)ceil(2.0f * Sigma);
	
	assert(BlurRadius <= MaxBlurRadius);
	
	std::vector<float> Weights;
	Weights.resize(2 * BlurRadius + 1);
	
	float WeightSum = 0.0f;
	for (int i = -BlurRadius; i <= BlurRadius; ++i)
	{
		float X = (float)i;

		Weights[i + BlurRadius] = expf(-X * X / TwoSigma2);

		WeightSum += Weights[i + BlurRadius];
	}
	// Divide by the sum so all the weights add up to 1.0.
	for (int i = 0; i < Weights.size(); ++i)
	{
		Weights[i] /= WeightSum;
	}

	return Weights;
}

void FBlurFilter::BuildDescriptors(CD3DX12_CPU_DESCRIPTOR_HANDLE CpuDescriptor, CD3DX12_GPU_DESCRIPTOR_HANDLE GpuDescriptor, UINT DescriptorSize)
{
	Blur0CpuSrv = CpuDescriptor;
	Blur0CpuUav = CpuDescriptor.Offset(1, DescriptorSize);
	Blur1CpuSrv = CpuDescriptor.Offset(1, DescriptorSize);
	Blur1CpuUav = CpuDescriptor.Offset(1, DescriptorSize);
	
	Blur0GpuSrv = GpuDescriptor;
	Blur0GpuUav = GpuDescriptor.Offset(1, DescriptorSize);
	Blur1GpuSrv = GpuDescriptor.Offset(1, DescriptorSize);
	Blur1GpuUav = GpuDescriptor.Offset(1, DescriptorSize);
	
	BuildDescriptors();
}

void FBlurFilter::BuildDescriptors()
{
	D3D12_SHADER_RESOURCE_VIEW_DESC SrvDesc = {};
	SrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	SrvDesc.Format = Format;
	SrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	SrvDesc.Texture2D.MostDetailedMip = 0;
	SrvDesc.Texture2D.MipLevels = 1;

	D3D12_UNORDERED_ACCESS_VIEW_DESC UavDesc = {};

	UavDesc.Format = Format;
	UavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	UavDesc.Texture2D.MipSlice = 0;

	Device->CreateShaderResourceView(BlurMap0.Get(), &SrvDesc, Blur0CpuSrv);
	Device->CreateShaderResourceView(BlurMap1.Get(), &SrvDesc, Blur1CpuSrv);
	
	Device->CreateUnorderedAccessView(BlurMap0.Get(), nullptr, &UavDesc, Blur0CpuUav);
	Device->CreateUnorderedAccessView(BlurMap1.Get(), nullptr, &UavDesc, Blur1CpuUav);
}

void FBlurFilter::BuildResources()
{
	// Note, compressed formats cannot be used for UAV.  We get error like:
	// ERROR: ID3D11Device::CreateTexture2D: The format (0x4d, BC3_UNORM) 
	// cannot be bound as an UnorderedAccessView, or cast to a format that
	// could be bound as an UnorderedAccessView.  Therefore this format 
	// does not support D3D11_BIND_UNORDERED_ACCESS.
	
	D3D12_RESOURCE_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(D3D12_RESOURCE_DESC));
	texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texDesc.Alignment = 0;
	texDesc.Width = Width;
	texDesc.Height = Height;
	texDesc.DepthOrArraySize = 1;
	texDesc.MipLevels = 1;
	texDesc.Format = Format;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	ThrowIfFailed(
		Device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&texDesc,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(&BlurMap0)
		)
	);

	ThrowIfFailed(
		Device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&texDesc,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(&BlurMap1)
		)
	);
}

void FBlurFilter::OnResize(UINT NewWidth, UINT NewHeight)
{
	if (Width != NewWidth || Height != NewHeight)
	{
		Width = NewWidth;
		Height = NewHeight;
		
		BuildResources();

		BuildDescriptors();
	}

}

void FBlurFilter::Execute(ID3D12GraphicsCommandList* CmdList, ID3D12RootSignature* RootSig, ID3D12PipelineState* HorzBlurPSO, ID3D12PipelineState* VertBlurPSO, ID3D12Resource* Input, int BlurCount)
{
	auto Weights = CalcGaussWeights(2.5f);
	int BlurRadius = (int)Weights.size() / 2;
	
	CmdList->SetComputeRootSignature(RootSig);
	CmdList->SetComputeRoot32BitConstants(0, 1, &BlurRadius, 0);
	CmdList->SetComputeRoot32BitConstants(0, (UINT)Weights.size(), Weights.data(), 1);
	
	CmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(Input,
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_SOURCE));
	
	CmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(Input,
		D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));
	
	// Copy the input (back-buffer in this example) to BlurMap0.
	CmdList->CopyResource(BlurMap0.Get(), Input);

	CmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(BlurMap0.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));

	CmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(BlurMap1.Get(),
		D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS));

	for (int i = 0; i < BlurCount; ++i)
	{
		//
		// Horizontal Blur pass.
		//
		CmdList->SetPipelineState(HorzBlurPSO);
		CmdList->SetComputeRootDescriptorTable(1, Blur0GpuSrv);
		CmdList->SetComputeRootDescriptorTable(2, Blur1GpuUav);
	
		// How many groups do we need to dispatch to cover a row of pixels, where each
		// group covers 256 pixels (the 256 is defined in the ComputeShader).
		UINT NumGroupsX = (UINT)ceilf(Width / 256.0f);
		CmdList->Dispatch(NumGroupsX, Height, 1);
		
		CmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(BlurMap0.Get(),
			D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS));

		CmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(BlurMap1.Get(),
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_GENERIC_READ));

		//
		// Vertical Blur pass.
		//
		CmdList->SetPipelineState(VertBlurPSO);
		
		CmdList->SetComputeRootDescriptorTable(1, Blur1GpuSrv);
		CmdList->SetComputeRootDescriptorTable(2, Blur0GpuUav);


		// How many groups do we need to dispatch to cover a column of pixels, where each
		// group covers 256 pixels  (the 256 is defined in the ComputeShader).
		UINT NumGroupsY = (UINT)ceilf(Height / 256.0f);
		CmdList->Dispatch(Width, NumGroupsY, 1);
		
		CmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(BlurMap1.Get(),
			D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS));

		CmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(BlurMap0.Get(),
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_GENERIC_READ));
	}
}


