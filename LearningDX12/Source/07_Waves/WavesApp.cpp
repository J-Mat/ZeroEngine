/*
 * @Author: J_Mat
 * @Date: 2022-05-24 17:22:36
 * @Last Modified by:   J_Mat
 * @Last Modified time: 2022-05-24 17:22:36
 */
#include "d3dApp.h"
#include <DirectXColors.h>
#include <MathHelper.h>
#include <UploadBuffer.h>
#include <iostream>
#include "GeometryGenerator.h"
#include "FrameResource.h"
#include "Waves.h"

using namespace DirectX;
using namespace Microsoft::WRL;
// using namespace DirectX::PackedVector;

const int gNumFrameResources = 3;

#define ITEM_SHAPE "Shape"
#define ITEM_BOX "Box"
#define ITEM_GRID "Grid"
#define ITEM_SPHERE "Sphere"
#define ITEM_CYLINDER "Cylinder"

// Lightweight structure stores parameters to draw a shape.  This will
// vary from app-to-app.
struct FRenderItem
{
	FRenderItem() = default;
	XMFLOAT4X4 World = MathHelper::Identity4x4();

	// Dirty flag indicating the object data has changed and we need to update the constant buffer.
	// Because we have an object cbuffer for each FrameResource, we have to apply the
	// update to each FrameResource.  Thus, when we modify obect data we should set
	// NumFramesDirty = gNumFrameResources so that each frame resource gets the update.
	int NumFrameDirty = gNumFrameResources;

	// Index into GPU constant buffer corresponding to the ObjectCB for this render item.
	UINT ObjCBIndex = -1;

	FMeshGeometry *Geo = nullptr;

	// Primitive topology.
	D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// DrawIndexedInstanced parameters.
	UINT IndexCount = 0;
	UINT StartIndexLocation = 0;
	int BaseVertexLocation = 0;
};

enum class URenderLayer : int
{
	Opaque = 0,
	Count
};

class FTexWavesApp : public FD3DApp
{
public:
	FTexWavesApp(HINSTANCE hInstance);
	~FTexWavesApp();

	virtual bool Initialize() override;

private:
	virtual void OnResize() override;
	virtual void Update(const GameTimer &gt) override;
	virtual void Draw(const GameTimer &gt) override;

	virtual void OnMouseDown(WPARAM btnState, int x, int y) override;
	virtual void OnMouseUp(WPARAM btnState, int x, int y) override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y) override;

	void OnKeyboardInput(const GameTimer &gt);
	void UpdateCamera(const GameTimer &gt);
	void UpdateObjectCBs(const GameTimer &gt);
	void UpdateMainPassCB(const GameTimer &gt);
	void UpdateWaves(const GameTimer& gt);

	void BuildRootSignature();
	void BuildShadersAndInputLayout();
	void BuildLandGeometry();
	void BuildWavesGeometryBuffers();
	void BuildPSOs();
	void BuildFrameResources();
	void BuildRenderItems();
	void DrawRenderItems(ID3D12GraphicsCommandList *cmdList, const std::vector<FRenderItem *> &ritems);

	float GetHillsHeight(float x, float z) const;
	XMFLOAT3 GetHillsNormal(float x, float z) const;

private:
	std::vector<std::unique_ptr<FFrameResource>> FrameResources;
	FFrameResource *CurrentFrameResource = nullptr;
	int CurrentFrameResourceIndex = 0;

	ComPtr<ID3D12RootSignature> RootSignature = nullptr;
	ComPtr<ID3D12DescriptorHeap> CBVHeap = nullptr;

	ComPtr<ID3D12DescriptorHeap> SrvDescriptorHeap = nullptr;

	std::unordered_map<std::string, std::unique_ptr<FMeshGeometry>> Geometries;
	std::unordered_map<std::string, ComPtr<ID3DBlob>> Shaders;
	std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> PSOs;

	std::vector<D3D12_INPUT_ELEMENT_DESC> InputLayout;


	FRenderItem* WavesRenderItem = nullptr;

	// List of all the render items.
	std::vector<std::unique_ptr<FRenderItem>> AllRenderItems;

	// Render items divided by PSO.
	std::vector<FRenderItem *> RenderLayers[(int)URenderLayer::Count];

	std::unique_ptr<FWaves> Waves;

	FPassConstants MainPassCB;
	UINT PassCBVOffset = 0;

	bool bIsWireFrame = false;

	XMFLOAT3 EyePos = {0.0f, 0.0f, 0.0f};
	XMFLOAT4X4 View = MathHelper::Identity4x4();
	XMFLOAT4X4 Proj = MathHelper::Identity4x4();

	float Theta = 1.5f * XM_PI;
	float Phi = XM_PIDIV4;
	float Radius = 50.0f;

	POINT LastMousePos;
};

FTexWavesApp::FTexWavesApp(HINSTANCE hInstance)
	: FD3DApp(hInstance)
{
}

FTexWavesApp::~FTexWavesApp()
{
}

float FTexWavesApp::GetHillsHeight(float X, float Z) const
{
	return 0.3f * (Z * sinf(0.1f * X) + X * cosf(0.1f * Z));
}

XMFLOAT3 FTexWavesApp::GetHillsNormal(float X, float Z) const
{
	XMFLOAT3 N(
		-0.03f * Z * cosf(0.1f * X) - 0.3f * cosf(0.1f * Z),
		1.0f,
		-0.3f * sinf(0.1f * X) + 0.03f * X * sinf(0.1f * Z));

	XMVECTOR UnitNormal = XMVector3Normalize(XMLoadFloat3(&N));
	XMStoreFloat3(&N, UnitNormal);

	return N;
}

bool FTexWavesApp::Initialize()
{
	if (!FD3DApp::Initialize())
	{
		return false;
	}

	// Reset the command list to prep for initialization commands.
	ThrowIfFailed(CommandList->Reset(DirectCmdListAlloctor.Get(), nullptr));
	
	Waves = std::make_unique<FWaves>(128, 128, 1.0f, 0.03f, 4.0f, 0.2f);

	BuildRootSignature();
	BuildShadersAndInputLayout();
	BuildLandGeometry();
	BuildWavesGeometryBuffers();
	BuildRenderItems();
	BuildFrameResources();
	BuildPSOs();

	// Execute the initialization commands.
	ThrowIfFailed(CommandList->Close());
	ID3D12CommandList *CmdsLists[] = {CommandList.Get()};
	CommandQueue->ExecuteCommandLists(_countof(CmdsLists), CmdsLists);

	// Wait until initialization is complete.
	FlushCommandQueue();

	return true;
}

void FTexWavesApp::OnResize()
{
	FD3DApp::OnResize();

	// The window resized, so update the aspect ratio and recompute the projection matrix.
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	DirectX::XMStoreFloat4x4(&Proj, P);
}

void FTexWavesApp::Update(const GameTimer &gt)
{
	OnKeyboardInput(gt);
	UpdateCamera(gt);

	CurrentFrameResourceIndex = (CurrentFrameResourceIndex + 1) % gNumFrameResources;
	CurrentFrameResource = FrameResources[CurrentFrameResourceIndex].get();

	// Has the GPU finished processing the commands of the current frame resource?
	// If not, wait until the GPU has completed commands up to this fence point.
	if (CurrentFrameResource->Fence != 0 &&
		Fence->GetCompletedValue() < CurrentFrameResource->Fence)
	{
		HANDLE EventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
		ThrowIfFailed(Fence->SetEventOnCompletion(CurrentFrameResource->Fence, EventHandle));
		WaitForSingleObject(EventHandle, INFINITE);
		CloseHandle(EventHandle);
	}

	UpdateObjectCBs(gt);
	UpdateMainPassCB(gt);
	UpdateWaves(gt);
}

void FTexWavesApp::Draw(const GameTimer &gt)
{
	auto CmdListAlloc = CurrentFrameResource->CommandListAllocator;

	// Reuse the memory associated with command recording.
	// We can only reset when the associated command lists have finished execution on th
	ThrowIfFailed(CmdListAlloc->Reset());

	// A command list can be reset after it has been added to the command queue via ExecuteCommandList.
	// Reusing the command list reuses memory.

	if (bIsWireFrame)
	{
		ThrowIfFailed(CommandList->Reset(CmdListAlloc.Get(), PSOs["opaque_wireframe"].Get()));
	}
	else
	{
		ThrowIfFailed(CommandList->Reset(CmdListAlloc.Get(), PSOs["opaque"].Get()));
	}

	CommandList->RSSetViewports(1, &ScreenViewport);
	CommandList->RSSetScissorRects(1, &ScissorRect);

	CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
										GetCurrentBackBuffer(),
										D3D12_RESOURCE_STATE_PRESENT,
										D3D12_RESOURCE_STATE_RENDER_TARGET));

	// Clear the back buffer and depth buffer.
	CommandList->ClearRenderTargetView(GetCurrentBackBufferView(), Colors::AliceBlue, 0, nullptr);
	CommandList->ClearDepthStencilView(GetDepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	// Specify the buffers we are going to render to.
	CommandList->OMSetRenderTargets(1, &GetCurrentBackBufferView(), true, &GetDepthStencilView());

	CommandList->SetGraphicsRootSignature(RootSignature.Get());

	// Bind per-pass constant buffer.  We only need to do this once per-pass.
	auto PassCB = CurrentFrameResource->PassCB->Resource();
	CommandList->SetGraphicsRootConstantBufferView(1, PassCB->GetGPUVirtualAddress());
	
	DrawRenderItems(CommandList.Get(), RenderLayers[(int)URenderLayer::Opaque]);


	// Indicate a state transition on the resource usage.
	CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
										GetCurrentBackBuffer(),
										D3D12_RESOURCE_STATE_RENDER_TARGET,
										D3D12_RESOURCE_STATE_PRESENT));

	// Done recording commands.
	ThrowIfFailed(CommandList->Close());

	// Add the command list to the queue for execution.
	ID3D12CommandList *CmdsLists[] = {CommandList.Get()};
	CommandQueue->ExecuteCommandLists(_countof(CmdsLists), CmdsLists);

	// Swap the back and front buffers
	ThrowIfFailed(SwapChain->Present(0, 0));
	CurrentBackBufferIndex = (CurrentBackBufferIndex + 1) % SwapChainBufferCount;

	// Advance the fence value to mark commands up to this fence point.
	CurrentFrameResource->Fence = ++CurrentFence;

	// Add an instruction to the command queue to set a new fence point.
	// Because we are on the GPU timeline, the new fence point won't be
	// set until the GPU finishes processing all the commands prior to this Signal().
	CommandQueue->Signal(Fence.Get(), CurrentFence);
}

void FTexWavesApp::OnMouseDown(WPARAM btnState, int x, int y)
{
	LastMousePos.x = x;
	LastMousePos.y = y;

	SetCapture(MainWnd);
}

void FTexWavesApp::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void FTexWavesApp::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f * static_cast<float>(x - LastMousePos.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(y - LastMousePos.y));

		// Update angles based on input to orbit camera around box.
		Theta -= dx;
		Phi -= dy;

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

void FTexWavesApp::OnKeyboardInput(const GameTimer &gt)
{
	if (GetAsyncKeyState('1') & 0x8000)
		bIsWireFrame = true;
	else
		bIsWireFrame = false;
}

void FTexWavesApp::UpdateCamera(const GameTimer &gt)
{
	// Convert Spherical to Cartesian coordinates.
	EyePos.x = Radius * sinf(Phi) * cosf(Theta);
	EyePos.z = Radius * sinf(Phi) * sinf(Theta);
	EyePos.y = Radius * cosf(Phi);

	// Build the view matrix.
	XMVECTOR pos = XMVectorSet(EyePos.x, EyePos.y, EyePos.z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&View, view);
}

void FTexWavesApp::UpdateObjectCBs(const GameTimer &gt)
{
	auto CurrentObjectCB = CurrentFrameResource->ObjectCB.get();
	for (auto &Item : AllRenderItems)
	{
		if (Item->NumFrameDirty > 0)
		{
			XMMATRIX World = XMLoadFloat4x4(&Item->World);

			FObjectConstants ObjConstants;
			XMStoreFloat4x4(&ObjConstants.World, XMMatrixTranspose(World));

			CurrentObjectCB->CopyData(Item->ObjCBIndex, ObjConstants);

			Item->NumFrameDirty--;
		}
	}
}

void FTexWavesApp::UpdateMainPassCB(const GameTimer &gt)
{
	XMMATRIX ViewMatrix = XMLoadFloat4x4(&View);
	XMMATRIX ProjMatrix = XMLoadFloat4x4(&Proj);

	XMMATRIX ViewProj = XMMatrixMultiply(ViewMatrix, ProjMatrix);
	XMMATRIX InvView = XMMatrixInverse(&XMMatrixDeterminant(ViewMatrix), ViewMatrix);
	XMMATRIX InvProj = XMMatrixInverse(&XMMatrixDeterminant(ProjMatrix), ProjMatrix);
	XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(ViewProj), ViewProj);

	XMStoreFloat4x4(&MainPassCB.View, XMMatrixTranspose(ViewMatrix));
	XMStoreFloat4x4(&MainPassCB.InvView, XMMatrixTranspose(InvView));
	XMStoreFloat4x4(&MainPassCB.Proj, XMMatrixTranspose(ProjMatrix));
	XMStoreFloat4x4(&MainPassCB.InvProj, XMMatrixTranspose(InvProj));
	XMStoreFloat4x4(&MainPassCB.ViewProj, XMMatrixTranspose(ViewProj));
	XMStoreFloat4x4(&MainPassCB.InvViewProj, XMMatrixTranspose(invViewProj));
	MainPassCB.EyePosW = EyePos;
	MainPassCB.RenderTargetSize = XMFLOAT2((float)ClientWidth, (float)ClientHeight);
	MainPassCB.InvRenderTargetSize = XMFLOAT2(1.0f / ClientWidth, 1.0f / ClientHeight);
	MainPassCB.NearZ = 1.0f;
	MainPassCB.FarZ = 1000.0f;
	MainPassCB.TotalTime = gt.TotalTime();
	MainPassCB.DeltaTime = gt.DeltaTime();

	auto CurPassCB = CurrentFrameResource->PassCB.get();
	CurPassCB->CopyData(0, MainPassCB);
}

void FTexWavesApp::UpdateWaves(const GameTimer& gt)
{
	static float t_base = 0.0f;
	if((Timer.TotalTime() - t_base) >= 0.25f)
	{
		t_base += 0.25f;

		int i = MathHelper::Rand(4, Waves->RowCount() - 5);
		int j = MathHelper::Rand(4, Waves->ColumnCount() - 5);

		float r = MathHelper::RandF(0.2f, 0.5f);

		Waves->Disturb(i, j, r);
	}

	// Update the wave simulation.
	Waves->Update(gt.DeltaTime());

	// Update the wave vertex buffer with the new solution.
	auto CurrWavesVB = CurrentFrameResource->WavesVB.get();
	for(int i = 0; i < Waves->VertexCount(); ++i)
	{
		FVertex v;

		v.Pos = Waves->Position(i);
        v.Color = XMFLOAT4(DirectX::Colors::Blue);

		CurrWavesVB->CopyData(i, v);
	}

	// Set the dynamic VB of the wave renderitem to the current frame VB.
	WavesRenderItem->Geo->VertexBufferGPU = CurrWavesVB->Resource();
}

void FTexWavesApp::BuildRootSignature()
{
	// Root parameter can be a table, root descriptor or root constants.
	CD3DX12_ROOT_PARAMETER SlotRootParameter[2];

	// Create root CBV.
	SlotRootParameter[0].InitAsConstantBufferView(0);
	SlotRootParameter[1].InitAsConstantBufferView(1);

	// A root signature is an array of root parameters.
	CD3DX12_ROOT_SIGNATURE_DESC RootSigDesc(2, SlotRootParameter, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	// create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
	ComPtr<ID3DBlob> SerializedRootSig = nullptr;
	ComPtr<ID3DBlob> ErrorBlob = nullptr;
	HRESULT HR = D3D12SerializeRootSignature(&RootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
											 SerializedRootSig.GetAddressOf(), ErrorBlob.GetAddressOf());

	if (ErrorBlob != nullptr)
	{
		::OutputDebugStringA((char *)ErrorBlob->GetBufferPointer());
	}
	ThrowIfFailed(HR);

	ThrowIfFailed(
		D3DDevice->CreateRootSignature(
			0,
			SerializedRootSig->GetBufferPointer(),
			SerializedRootSig->GetBufferSize(),
			IID_PPV_ARGS(RootSignature.GetAddressOf())));
}

void FTexWavesApp::BuildShadersAndInputLayout()
{
	HRESULT hr = S_OK;
	Shaders["standardVS"] = d3dUtil::CompileShader(L"Shaders\\Shape.hlsl", nullptr, "VS", "vs_5_0");
	Shaders["opaquePS"] = d3dUtil::CompileShader(L"Shaders\\Shape.hlsl", nullptr, "PS", "ps_5_0");

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

void FTexWavesApp::BuildLandGeometry()
{
	FGeometryGenerator GeometryGenerator;
	FGeometryGenerator::FMeshData Grid = GeometryGenerator.CreateGrid(160.0f, 160.0f, 50, 50);

	//
	// Extract the vertex elements we are interested and apply the height function to
	// each vertex.  In addition, color the vertices based on their height so we have
	// sandy looking beaches, grassy low hills, and snow mountain peaks.
	//

	std::vector<FVertex> Vertices(Grid.Vertices.size());
	for (size_t i = 0; i < Grid.Vertices.size(); ++i)
	{
		auto &p = Grid.Vertices[i].Position;
		Vertices[i].Pos = p;
		Vertices[i].Pos.y = GetHillsHeight(p.x, p.z);

		// Color the vertex based on its height.
		if (Vertices[i].Pos.y < -10.0f)
		{
			// Sandy beach color.
			Vertices[i].Color = XMFLOAT4(1.0f, 0.96f, 0.62f, 1.0f);
		}
		else if (Vertices[i].Pos.y < 5.0f)
		{
			// Light yellow-green.
			Vertices[i].Color = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
		}
		else if (Vertices[i].Pos.y < 12.0f)
		{
			// Dark yellow-green.
			Vertices[i].Color = XMFLOAT4(0.1f, 0.48f, 0.19f, 1.0f);
		}
		else if (Vertices[i].Pos.y < 20.0f)
		{
			// Dark brown.
			Vertices[i].Color = XMFLOAT4(0.45f, 0.39f, 0.34f, 1.0f);
		}
		else
		{
			// White snow.
			Vertices[i].Color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		}
	}

	const UINT VBByteSize = (UINT)Vertices.size() * sizeof(FVertex);

	std::vector<std::uint16_t> Indices = Grid.GetIndices16();
	const UINT IBByteSize = (UINT)Indices.size() * sizeof(std::uint16_t);

	auto Geo = std::make_unique<FMeshGeometry>();
	Geo->Name = "landGeo";

	ThrowIfFailed(D3DCreateBlob(VBByteSize, &Geo->VertexBufferCPU));
	CopyMemory(Geo->VertexBufferCPU->GetBufferPointer(), Vertices.data(), VBByteSize);

	ThrowIfFailed(D3DCreateBlob(IBByteSize, &Geo->IndexBufferCPU));
	CopyMemory(Geo->IndexBufferCPU->GetBufferPointer(), Indices.data(), IBByteSize);

	Geo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(D3DDevice.Get(), CommandList.Get(), Vertices.data(), VBByteSize, Geo->VertexBufferUploader);
	Geo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(D3DDevice.Get(), CommandList.Get(), Indices.data(), IBByteSize, Geo->IndexBufferUploader);

	Geo->VertexByteStride = sizeof(FVertex);
	Geo->VertexBufferByteSize = VBByteSize;
	Geo->IndexFormat = DXGI_FORMAT_R16_UINT;
	Geo->IndexBufferByteSize = IBByteSize;

	FSubmeshGeometry SubMesh;
	SubMesh.IndexCount = (UINT)Indices.size();
	SubMesh.StartIndexLocation = 0;
	SubMesh.BaseVertexLocation = 0;

	Geo->DrawArgs["grid"] = SubMesh;
	Geometries["landGeo"] = std::move(Geo);
}

void FTexWavesApp::BuildWavesGeometryBuffers()
{
	std::vector<std::uint16_t> indices(3 * Waves->TriangleCount()); // 3 indices per face
	assert(Waves->VertexCount() < 0x0000ffff);

	// Iterate over each quad.
	int m = Waves->RowCount();
	int n = Waves->ColumnCount();
	int k = 0;
	for(int i = 0; i < m - 1; ++i)
	{
		for(int j = 0; j < n - 1; ++j)
		{
			indices[k] = i*n + j;
			indices[k + 1] = i*n + j + 1;
			indices[k + 2] = (i + 1)*n + j;

			indices[k + 3] = (i + 1)*n + j;
			indices[k + 4] = i*n + j + 1;
			indices[k + 5] = (i + 1)*n + j + 1;

			k += 6; // next quad
		}
	}

	UINT vbByteSize = Waves->VertexCount()*sizeof(FVertex);
	UINT ibByteSize = (UINT)indices.size()*sizeof(std::uint16_t);

	auto geo = std::make_unique<FMeshGeometry>();
	geo->Name = "waterGeo";

	// Set dynamically.
	geo->VertexBufferCPU = nullptr;
	geo->VertexBufferGPU = nullptr;

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	geo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(D3DDevice.Get(),
		CommandList.Get(), indices.data(), ibByteSize, geo->IndexBufferUploader);

	geo->VertexByteStride = sizeof(FVertex);
	geo->VertexBufferByteSize = vbByteSize;
	geo->IndexFormat = DXGI_FORMAT_R16_UINT;
	geo->IndexBufferByteSize = ibByteSize;

	FSubmeshGeometry Submesh;
	Submesh.IndexCount = (UINT)indices.size();
	Submesh.StartIndexLocation = 0;
	Submesh.BaseVertexLocation = 0;

	geo->DrawArgs["grid"] = Submesh;

	Geometries["waterGeo"] = std::move(geo);
}

void FTexWavesApp::BuildRenderItems()
{
	auto WavesRitem = std::make_unique<FRenderItem>();
	WavesRitem->World = MathHelper::Identity4x4();
	WavesRitem->ObjCBIndex = 0;
	WavesRitem->Geo = Geometries["waterGeo"].get();
	WavesRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	WavesRitem->IndexCount = WavesRitem->Geo->DrawArgs["grid"].IndexCount;
	WavesRitem->StartIndexLocation = WavesRitem->Geo->DrawArgs["grid"].StartIndexLocation;
	WavesRitem->BaseVertexLocation = WavesRitem->Geo->DrawArgs["grid"].BaseVertexLocation;

	WavesRenderItem = WavesRitem.get();

	RenderLayers[(int)URenderLayer::Opaque].push_back(WavesRitem.get());

	auto GridRitem = std::make_unique<FRenderItem>();
	GridRitem->World = MathHelper::Identity4x4();
	GridRitem->ObjCBIndex = 1;
	GridRitem->Geo = Geometries["landGeo"].get();
	GridRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	GridRitem->IndexCount = GridRitem->Geo->DrawArgs["grid"].IndexCount;
	GridRitem->StartIndexLocation = GridRitem->Geo->DrawArgs["grid"].StartIndexLocation;
	GridRitem->BaseVertexLocation = GridRitem->Geo->DrawArgs["grid"].BaseVertexLocation;

	RenderLayers[(int)URenderLayer::Opaque].push_back(GridRitem.get());

	AllRenderItems.push_back(std::move(WavesRitem));
	AllRenderItems.push_back(std::move(GridRitem));
}

void FTexWavesApp::DrawRenderItems(ID3D12GraphicsCommandList *CommandList, const std::vector<FRenderItem *> &RenderItems)
{
	UINT ObjCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(FObjectConstants));
	auto ObjectCB = CurrentFrameResource->ObjectCB->Resource();

	for (int i = 0; i < RenderItems.size(); ++i)
	{
		auto* Item = RenderItems[i];
		
		CommandList->IASetVertexBuffers(0, 1, &Item->Geo->VertexBufferView());
		CommandList->IASetIndexBuffer(&Item->Geo->IndexBufferView());
		CommandList->IASetPrimitiveTopology(Item->PrimitiveType);
		
		D3D12_GPU_VIRTUAL_ADDRESS ObjCBAddress = ObjectCB->GetGPUVirtualAddress();
		ObjCBAddress += Item->ObjCBIndex * ObjCBByteSize;
	
		CommandList->SetGraphicsRootConstantBufferView(0, ObjCBAddress);
		
		CommandList->DrawIndexedInstanced(Item->IndexCount, 1, Item->StartIndexLocation, Item->BaseVertexLocation, 0);
	}
}

void FTexWavesApp::BuildFrameResources()
{
	for (int i = 0; i < gNumFrameResources; ++i)
	{
		FrameResources.push_back(
			std::make_unique<FFrameResource>(
				D3DDevice.Get(),
				1,
				(UINT)AllRenderItems.size(),
				Waves->VertexCount()));
	}
}

void FTexWavesApp::BuildPSOs()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC OpaquePSODesc;

	//
	// PSO for opaque objects.
	//
	ZeroMemory(&OpaquePSODesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	OpaquePSODesc.InputLayout = {InputLayout.data(), (UINT)InputLayout.size()};
	OpaquePSODesc.pRootSignature = RootSignature.Get();
	OpaquePSODesc.VS =
	{
		reinterpret_cast<BYTE *>(Shaders["standardVS"]->GetBufferPointer()),
		Shaders["standardVS"]->GetBufferSize()
	};
	OpaquePSODesc.PS =
	{
		reinterpret_cast<BYTE *>(Shaders["opaquePS"]->GetBufferPointer()),
		Shaders["opaquePS"]->GetBufferSize()
	};
	OpaquePSODesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	OpaquePSODesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	OpaquePSODesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	OpaquePSODesc.SampleMask = UINT_MAX;
	OpaquePSODesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	OpaquePSODesc.NumRenderTargets = 1;
	OpaquePSODesc.RTVFormats[0] = BackBufferFormat;
	OpaquePSODesc.SampleDesc.Count = X4MSAAState ? 4 : 1;
	OpaquePSODesc.SampleDesc.Quality = X4MSAAState ? (X4MSAAQuality - 1) : 0;
	OpaquePSODesc.DSVFormat = DepthStencilFormat;
	ThrowIfFailed(
		D3DDevice->CreateGraphicsPipelineState(&OpaquePSODesc, IID_PPV_ARGS(&PSOs["opaque"])));

	//
	// PSO for opaque wireframe objects.
	//
	D3D12_GRAPHICS_PIPELINE_STATE_DESC opaqueWireframePsoDesc = OpaquePSODesc;
	opaqueWireframePsoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	ThrowIfFailed(
		D3DDevice->CreateGraphicsPipelineState(&opaqueWireframePsoDesc, IID_PPV_ARGS(&PSOs["opaque_wireframe"])));
}

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	try
	{
		FTexWavesApp App(hInst);
		if (!App.Initialize())
		{
			return 0;
		}
		return App.Run();
	}
	catch (DxException &e)
	{
		MessageBox(0, e.ToString().c_str(), 0, 0);
		return 0;
	}
	return 0;
}
