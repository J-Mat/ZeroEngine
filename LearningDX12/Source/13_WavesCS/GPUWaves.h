//***************************************************************************************
// GpuWaves.h by Frank Luna (C) 2011 All Rights Reserved.
//
// Performs the calculations for the wave simulation using the ComputeShader on the GPU.  
// The solution is saved to a floating-point texture.  The client must then set this 
// texture as a SRV and do the displacement mapping in the vertex shader over a grid.
//***************************************************************************************

#ifndef GPUWAVES_H
#define GPUWAVES_H

#include "d3dUtil.h"
#include "GameTimer.h"

using namespace DirectX;
using namespace Microsoft::WRL;

class FGPUWaves
{
public:
	// Note that m,n should be divisible by 16 so there is no 
	// remainder when we divide into thread groups.
	FGPUWaves(ID3D12Device* Device, ID3D12GraphicsCommandList* CmdList, int m, int n, float dx, float dt, float Speed, float Damping);
	FGPUWaves(const FGPUWaves& rhs) = delete;
	FGPUWaves& operator=(const FGPUWaves& rhs) = delete;
	~FGPUWaves() = default;

	UINT RowCount()const;
	UINT ColumnCount()const;
	UINT GetVertexCount()const;
	UINT GetTriangleCount()const;
	float Width() const;
	float Depth() const;
	float GetSpatialStep() const;

	CD3DX12_GPU_DESCRIPTOR_HANDLE DisplacementMap()const;

	UINT DescriptorCount()const;

	void BuildResources(ID3D12GraphicsCommandList * CmdList);

	void BuildDescriptors(
		CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuDescriptor,
		CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuDescriptor,
		UINT DescriptorSize);

	void Update(
		const GameTimer & gt,
		ID3D12GraphicsCommandList * CmdList,
		ID3D12RootSignature * RootSig,
		ID3D12PipelineState * PSO);

	void Disturb(
		ID3D12GraphicsCommandList * CmdList,
		ID3D12RootSignature * RootSig,
		ID3D12PipelineState * PSO,
		UINT i, UINT j,
		float Magnitude);

private:
	UINT NumRows;
	UINT NumCols;

	UINT VertexCount;
	UINT TriangleCount;

	// Simulation constants we can precompute.
	float K[3];

	float TimeStep;
	float SpatialStep;
	
	ID3D12Device* D3DDevice = nullptr;
	
	CD3DX12_GPU_DESCRIPTOR_HANDLE PrevSolSrv;
	CD3DX12_GPU_DESCRIPTOR_HANDLE CurrSolSrv;
	CD3DX12_GPU_DESCRIPTOR_HANDLE NextSolSrv;

	CD3DX12_GPU_DESCRIPTOR_HANDLE PrevSolUav;
	CD3DX12_GPU_DESCRIPTOR_HANDLE CurrSolUav;
	CD3DX12_GPU_DESCRIPTOR_HANDLE NextSolUav;
	

	// Two for ping-ponging the textures.
	ComPtr<ID3D12Resource> PrevSol = nullptr;
	ComPtr<ID3D12Resource> CurrSol = nullptr;
	ComPtr<ID3D12Resource> NextSol = nullptr;

	ComPtr<ID3D12Resource> PrevUploadBuffer = nullptr;
	ComPtr<ID3D12Resource> CurrUploadBuffer = nullptr;
};

#endif // GPUWAVES_H