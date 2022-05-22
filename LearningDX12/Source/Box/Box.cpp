#include "d3dApp.h"
#include <DirectXColors.h>
#include <MathHelper.h>
#include <UploadBuffer.h>
#include <iostream>

using namespace DirectX;
using namespace Microsoft::WRL;
using namespace DirectX::PackedVector;

struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

struct FObjectConstants
{
	XMFLOAT4X4 WorldViewProj = MathHelper::Identity4x4();
};

class FBoxApp : public FD3DApp
{
public:
	FBoxApp(HINSTANCE hInstance);
	~FBoxApp();

	virtual bool Initialize() override;

private:
	virtual void OnResize()override;
	virtual void Update(const GameTimer& gt)override;
	virtual void Draw(const GameTimer& gt)override;

	virtual void OnMouseDown(WPARAM btnState, int x, int y)override;
	virtual void OnMouseUp(WPARAM btnState, int x, int y)override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y)override;

	void BuildDescriptorHeaps();
	void BuildConstantBuffers();
	void BuildRootSignature();
	void BuildShadersAndInputLayout();
	void BuildBoxGeometry();
	void BuildPSO();
private:
	ComPtr<ID3D12RootSignature> RootSignature = nullptr;
	ComPtr<ID3D12DescriptorHeap> CBVHeap = nullptr;
	
	std::unique_ptr<TUploadBuffer<FObjectConstants>> ObjectCB = nullptr;
	
	std::unique_ptr<MeshGeometry> BoxGeo = nullptr;
	
	ComPtr<ID3DBlob> VSByteCode = nullptr;
	ComPtr<ID3DBlob> PSByteCode = nullptr;
	
	std::vector<D3D12_INPUT_ELEMENT_DESC> InputLayout;
	ComPtr<ID3D12PipelineState> PSO = nullptr;
	
	XMFLOAT4X4 World = MathHelper::Identity4x4();
	XMFLOAT4X4 View = MathHelper::Identity4x4();
	XMFLOAT4X4 Proj = MathHelper::Identity4x4();

	float Theta = 1.5f * XM_PI;
	float Phi = XM_PIDIV4;
	float Radius = 5.0f;

	POINT LastMousePos;
};


FBoxApp::FBoxApp(HINSTANCE hInstance)
	: FD3DApp(hInstance)
{

}

FBoxApp::~FBoxApp()
{

}

bool FBoxApp::Initialize()
{
	if (!FD3DApp::Initialize())
	{
		return false;
	}

	// Reset the command list to prep for initialization commands.
	ThrowIfFailed(
		CommandList->Reset(DirectCmdListAlloctor.Get(), nullptr)
	);
	
	BuildDescriptorHeaps();
	BuildConstantBuffers();
	BuildRootSignature();
	BuildShadersAndInputLayout();
	BuildBoxGeometry();
	BuildPSO();
	// Execute the initialization commands.
	ThrowIfFailed(CommandList->Close());
	ID3D12CommandList* CmdsLists[] = { CommandList.Get() };
	CommandQueue->ExecuteCommandLists(_countof(CmdsLists), CmdsLists);

	// Wait until initialization is complete.
	FlushCommandQueue();

	return true;
}

void FBoxApp::OnResize()
{
	FD3DApp::OnResize();
}

void FBoxApp::Update(const GameTimer& gt)
{
	// Convert Spherical to Cartesian coordinates.
	float X = Radius * sinf(Phi) * cosf(Theta);
	float Z = Radius * sinf(Phi) * sinf(Theta);
	float Y = Radius * cosf(Phi);
	
	XMVECTOR Pos = XMVectorSet( X, Y, Z, 1.0f );
	XMVECTOR Target = XMVectorZero();
	XMVECTOR Up = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
		
	XMMATRIX ViewMatrix = XMMatrixLookAtLH(Pos, Target, Up);
	XMStoreFloat4x4(&View, ViewMatrix);
	
	XMMATRIX WorldMatrix = XMLoadFloat4x4(&World);
	XMMATRIX ProjMatrix = XMLoadFloat4x4(&Proj);
	XMMATRIX WorldViewProj = WorldMatrix * ViewMatrix * ProjMatrix;

	// Update the constant buffer with the latest worldViewProj matrix.
	FObjectConstants ObjConstants;
	XMStoreFloat4x4(&ObjConstants.WorldViewProj, XMMatrixTranspose(WorldViewProj));
	ObjectCB->CopyData(0, ObjConstants);
}

void FBoxApp::Draw(const GameTimer& gt)
{
	// Reuse the memory associated with command recording.
	// We can only reset when the associated command lists have finished execution on the GPU.
	ThrowIfFailed(DirectCmdListAlloctor->Reset());


	// A command list can be reset after it has been added to the command queue via ExecuteCommandList.
	// Reusing the command list reuses memory.
	ThrowIfFailed(CommandList->Reset(DirectCmdListAlloctor.Get(), PSO.Get()));

	CommandList->RSSetViewports(1, &ScreenViewport);
	CommandList->RSSetScissorRects(1, &ScissorRect);
	
	// Indicate a state transition on the resource usage.	
	CommandList->ResourceBarrier(
		1, 
		&CD3DX12_RESOURCE_BARRIER::Transition(
			GetCurrentBackBuffer(),
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET
		)
	);
	
	// Clear the back buffer and depth buffer.
	CommandList->ClearRenderTargetView(GetCurrentBackBufferView(), Colors::LightSteelBlue, 0, nullptr);
	CommandList->ClearDepthStencilView(GetDepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	// Specify the buffers we are going to render to.
	CommandList->OMSetRenderTargets(1, &GetCurrentBackBufferView(), true, &GetDepthStencilView());
	
	ID3D12DescriptorHeap* DescriptorHeaps[] = { CBVHeap.Get() };
	CommandList->SetDescriptorHeaps(_countof(DescriptorHeaps), DescriptorHeaps);
	
	CommandList->SetGraphicsRootSignature(RootSignature.Get());
	
	CommandList->IASetVertexBuffers(0, 1, &BoxGeo->VertexBufferView());
	CommandList->IASetIndexBuffer(&BoxGeo->IndexBufferView());
	CommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	CommandList->SetGraphicsRootDescriptorTable(0, CBVHeap->GetGPUDescriptorHandleForHeapStart());
	
	CommandList->DrawIndexedInstanced(
		BoxGeo->DrawArgs["Box"].IndexCount,
		1, 0, 0, 0
	);

	// Indicate a state transition on the resource usage.	
	CommandList->ResourceBarrier(1, 
		&CD3DX12_RESOURCE_BARRIER::Transition(
			GetCurrentBackBuffer(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT
		)
	);
	
	// Done recording commands.
	ThrowIfFailed(CommandList->Close());
	
	ID3D12CommandList* CmdsLists[] = { CommandList.Get() };
	CommandQueue->ExecuteCommandLists(_countof(CmdsLists), CmdsLists);

	// swap the back and front buffers
	ThrowIfFailed(SwapChain->Present(0, 0));
	CurrentBackBufferIndex = (CurrentBackBufferIndex + 1) % SwapChainBufferCount;
	
	// Wait until frame commands are complete.  This waiting is inefficient and is
	// done for simplicity.  Later we will show how to organize our rendering code
	// so we do not have to wait per frame.
	FlushCommandQueue();
}



void FBoxApp::OnMouseDown(WPARAM btnState, int x, int y)
{
	LastMousePos.x = x;
	LastMousePos.y = y;
	
	SetCapture(MainWnd);
}

void FBoxApp::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void FBoxApp::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f * static_cast<float>(x - LastMousePos.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(y - LastMousePos.y));

		// Update angles based on input to orbit camera around box.
		Theta += dx;
		Phi += dy;

		// Restrict the angle mPhi.
		Phi = MathHelper::Clamp(Phi, 0.1f, MathHelper::Pi - 0.1f);
	}
	else if ((btnState & MK_RBUTTON) != 0)
	{
		// Make each pixel correspond to 0.005 unit in the scene.
		float dx = 0.005f * static_cast<float>(x - LastMousePos.x);
		float dy = 0.005f * static_cast<float>(y - LastMousePos.y);

		// Update the camera radius based on input.
		Radius += dx - dy;

		// Restrict the radius.
		Radius = MathHelper::Clamp(Radius, 3.0f, 15.0f);
	}

	LastMousePos.x = x;
	LastMousePos.y = y;
}

void FBoxApp::BuildDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC CBVHeapDesc;
	CBVHeapDesc.NumDescriptors = 1;
	CBVHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	CBVHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	CBVHeapDesc.NodeMask = 0;
	ThrowIfFailed(
		D3DDevice->CreateDescriptorHeap(
			&CBVHeapDesc,
			IID_PPV_ARGS(&CBVHeap)
		)
	);
}

void FBoxApp::BuildConstantBuffers()
{
	ObjectCB = std::make_unique<TUploadBuffer<FObjectConstants>>(D3DDevice.Get(), 1, true);

	UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(FObjectConstants));

	D3D12_GPU_VIRTUAL_ADDRESS cbAddress = ObjectCB->Resource()->GetGPUVirtualAddress();
	// Offset to the ith object constant buffer in the buffer.
	int boxCBufIndex = 0;
	cbAddress += boxCBufIndex * objCBByteSize;

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = cbAddress;
	cbvDesc.SizeInBytes = d3dUtil::CalcConstantBufferByteSize(sizeof(FObjectConstants));

	D3DDevice->CreateConstantBufferView(
		&cbvDesc,
		CBVHeap->GetCPUDescriptorHandleForHeapStart());
}

void FBoxApp::BuildRootSignature()
{
	// Shader programs typically require resources as input (constant buffers,
	// textures, samplers).  The root signature defines the resources the shader
	// programs expect.  If we think of the shader programs as a function, and
	// the input resources as function parameters, then the root signature can be
	// thought of as defining the function signature.  

	// Root parameter can be a table, root descriptor or root constants.
	CD3DX12_ROOT_PARAMETER  SlotRootParameter[1];
	
	// Create a single descriptor table of CBVs.
	CD3DX12_DESCRIPTOR_RANGE CBVTable;
	CBVTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
	SlotRootParameter[0].InitAsDescriptorTable(1, &CBVTable);
	
	// A root signature is an array of root parameters.
	CD3DX12_ROOT_SIGNATURE_DESC RootsigDesc(1, SlotRootParameter, 0, nullptr,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	// create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
	ComPtr<ID3DBlob> SerializedRootSig = nullptr;
	ComPtr<ID3DBlob> ErrorBlob = nullptr;

	HRESULT hr = D3D12SerializeRootSignature(&RootsigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		SerializedRootSig.GetAddressOf(), ErrorBlob.GetAddressOf());

	if (ErrorBlob != nullptr)
	{
		::OutputDebugStringA((char*)ErrorBlob->GetBufferPointer());
	}
	ThrowIfFailed(hr);

	ThrowIfFailed(D3DDevice->CreateRootSignature(
		0,
		SerializedRootSig->GetBufferPointer(),
		SerializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(&RootSignature))
	);
}

void FBoxApp::BuildShadersAndInputLayout()
{
	HRESULT hr = S_OK;

	VSByteCode = d3dUtil::CompileShader(L"Shaders//Color.hlsl", nullptr, "VS", "vs_5_0");
	PSByteCode = d3dUtil::CompileShader(L"Shaders//Color.hlsl", nullptr, "PS", "ps_5_0");


	/*
	typedef struct D3D12_INPUT_ELEMENT_DESC
	{
		LPCSTR SemanticName;
		UINT SemanticIndex;
		DXGI_FORMAT Format;
		UINT InputSlot;
		UINT AlignedByteOffset;
		D3D12_INPUT_CLASSIFICATION InputSlotClass;
		UINT InstanceDataStepRate;
	} 	D3D12_INPUT_ELEMENT_DESC;
	*/
	InputLayout =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"Color", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	};
}

void FBoxApp::BuildBoxGeometry()
{
	std::array<Vertex, 8> Vertices =
	{
		Vertex({ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::White) }),
		Vertex({ XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Black) }),
		Vertex({ XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Red) }),
		Vertex({ XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::Green) }),
		Vertex({ XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Blue) }),
		Vertex({ XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Yellow) }),
		Vertex({ XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Cyan) }),
		Vertex({ XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Magenta) })
	};
	
	std::array<std::uint16_t, 36> Indices =
	{
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3,
		4, 3, 7
	};
	
	const UINT VBByteSize = (UINT)Vertices.size() * sizeof(Vertex);
	const UINT IBByteSize = (UINT)Indices.size() * sizeof(std::uint16_t);

	BoxGeo = std::make_unique<MeshGeometry>();
	BoxGeo->Name = "Box";
	ThrowIfFailed(
		D3DCreateBlob(VBByteSize, &BoxGeo->VertexBufferCPU)
	);
	CopyMemory(BoxGeo->VertexBufferCPU->GetBufferPointer(), Vertices.data(), VBByteSize);

	ThrowIfFailed(
		D3DCreateBlob(IBByteSize, &BoxGeo->IndexBufferCPU)
	);
	CopyMemory(BoxGeo->IndexBufferCPU->GetBufferPointer(), Indices.data(), IBByteSize);
	
	BoxGeo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(
		D3DDevice.Get(),
		CommandList.Get(),
		Vertices.data(),
		VBByteSize,
		BoxGeo->VertexBufferUploader
	);
	
	BoxGeo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(
		D3DDevice.Get(),
		CommandList.Get(),
		Indices.data(),
		IBByteSize,
		BoxGeo->IndexBufferUploader
	);
	
	BoxGeo->VertexByteStride = sizeof(Vertex);
	BoxGeo->VertexBufferByteSize = VBByteSize;
	BoxGeo->IndexFormat = DXGI_FORMAT_R16_UINT;
	BoxGeo->IndexBufferByteSize = IBByteSize;

	SubmeshGeometry SubMesh;
	SubMesh.IndexCount = (UINT)Indices.size();
	SubMesh.StartIndexLocation = 0;
	SubMesh.BaseVertexLocation = 0;
	
	BoxGeo->DrawArgs["Box"] = SubMesh;
}

void FBoxApp::BuildPSO()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC PSODesc;
	ZeroMemory(&PSODesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	PSODesc.InputLayout = {InputLayout.data(), (UINT)InputLayout.size()};
	PSODesc.pRootSignature = RootSignature.Get();
	PSODesc.VS = {
		reinterpret_cast<BYTE*>(VSByteCode->GetBufferPointer()),
		VSByteCode->GetBufferSize()
	};
	PSODesc.PS = {
		reinterpret_cast<BYTE*>(PSByteCode->GetBufferPointer()),
		PSByteCode->GetBufferSize()
	};
	PSODesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	PSODesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	PSODesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	PSODesc.SampleMask = UINT_MAX;
	PSODesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	PSODesc.NumRenderTargets = 1;
	PSODesc.RTVFormats[0] = BackBufferFormat;
	PSODesc.SampleDesc.Count = X4MSAAState ? 4 : 1;
	PSODesc.SampleDesc.Quality = X4MSAAState ? (X4MSAAQuality - 1) : 0;
	PSODesc.DSVFormat = DepthStencilFormat;
	ThrowIfFailed(
		D3DDevice->CreateGraphicsPipelineState(
			&PSODesc, 
			IID_PPV_ARGS(&PSO)
		)
	);
}

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	
	try
	{
		FBoxApp App(hInst);
		if (!App.Initialize())
		{
			return  0;
		}
		return App.Run();
	}
	catch (DxException& e)
	{
		MessageBox(0, e.ToString().c_str(), 0, 0);
		return 0;
	}
	return 0;
}
