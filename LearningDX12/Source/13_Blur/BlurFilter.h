#pragma once

#include "d3dUtil.h"

class FBlurFilter
{
public:
	FBlurFilter(ID3D12Device* InDevice,
		UINT Width, UINT Height,
		DXGI_FORMAT Format);

	FBlurFilter(const FBlurFilter& rhs) = delete;
	FBlurFilter& operator=(const FBlurFilter& rhs) = delete;
	~FBlurFilter() = default;
	
	ID3D12Resource* Output();
	
	void BuildDescriptors(
		CD3DX12_CPU_DESCRIPTOR_HANDLE CpuDescriptor,
		CD3DX12_GPU_DESCRIPTOR_HANDLE GpuDescriptor,
		UINT DescriptorSize
	);

	void OnResize(UINT NewWidth, UINT NewHeight);
	
	void Execute(
		ID3D12GraphicsCommandList* CmdList,
		ID3D12RootSignature* RootSig,
		ID3D12PipelineState* HorzBlurPSO,
		ID3D12PipelineState* VertBlurPSO,
		ID3D12Resource* Input,
		int BlurCount);

private:
	const int MaxBlurRadius = 5;

	std::vector<float> CalcGaussWeights(float Sigma);

	void BuildDescriptors();
	void BuildResources();



	ID3D12Device* Device = nullptr;

	UINT Width = 0;
	UINT Height = 0;
	DXGI_FORMAT Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	CD3DX12_CPU_DESCRIPTOR_HANDLE Blur0CpuSrv;
	CD3DX12_CPU_DESCRIPTOR_HANDLE Blur0CpuUav;

	CD3DX12_CPU_DESCRIPTOR_HANDLE Blur1CpuSrv;
	CD3DX12_CPU_DESCRIPTOR_HANDLE Blur1CpuUav;

	CD3DX12_GPU_DESCRIPTOR_HANDLE Blur0GpuSrv;
	CD3DX12_GPU_DESCRIPTOR_HANDLE Blur0GpuUav;

	CD3DX12_GPU_DESCRIPTOR_HANDLE Blur1GpuSrv;
	CD3DX12_GPU_DESCRIPTOR_HANDLE Blur1GpuUav;

	// Two for ping-ponging the textures.
	Microsoft::WRL::ComPtr<ID3D12Resource> BlurMap0 = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> BlurMap1 = nullptr;
};
