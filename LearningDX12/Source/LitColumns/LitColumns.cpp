/*
 * @Author: J_Mat
 * @Date: 2022-06-04 17:50:34
 * @Last Modified by: J_Mat
 * @Last Modified time: 2022-06-04 19:25:26
 */

#include "d3dApp.h"
#include <DirectXColors.h>
#include <MathHelper.h>
#include <UploadBuffer.h>
#include <iostream>
#include "GeometryGenerator.h"
#include "FrameResource.h"

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
	XMFLOAT4X4 TexTransform = MathHelper::Identity4x4();

	// Dirty flag indicating the object data has changed and we need to update the constant buffer.
	// Because we have an object cbuffer for each FrameResource, we have to apply the
	// update to each FrameResource.  Thus, when we modify obect data we should set
	// NumFramesDirty = gNumFrameResources so that each frame resource gets the update.
	int NumFrameDirty = gNumFrameResources;

	// Index into GPU constant buffer corresponding to the ObjectCB for this render item.
	UINT ObjCBIndex = -1;

	FMaterial* Mat = nullptr;
	FMeshGeometry *Geo = nullptr;

	// Primitive topology.
	D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// DrawIndexedInstanced parameters.
	UINT IndexCount = 0;
	UINT StartIndexLocation = 0;
	int BaseVertexLocation = 0;
};

class FLitColumn : public FD3DApp
{
public:
	FLitColumn(HINSTANCE hInstance);
	~FLitColumn();

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
	void UpdateMaterialCBs(const GameTimer &gt);
	void UpdateMainPassCB(const GameTimer &gt);

	void BuildRootSignature();
	void BuildMaterials();
	void BuildShadersAndInputLayout();
	void BuildShapeGeometry();
	void BuildSkullGeometry();
	void BuildPSOs();
	void BuildFrameResources();
	void BuildRenderItems();
	void DrawRenderItems(ID3D12GraphicsCommandList *cmdList, const std::vector<FRenderItem *> &ritems);

private:
	std::vector<std::unique_ptr<FFrameResource>> FrameResources;
	FFrameResource *CurrentFrameResource = nullptr;
	int CurrentFrameResourceIndex = 0;

	ComPtr<ID3D12RootSignature> RootSignature = nullptr;
	ComPtr<ID3D12DescriptorHeap> CBVHeap = nullptr;

	ComPtr<ID3D12DescriptorHeap> SrvDescriptorHeap = nullptr;

	std::unordered_map<std::string, std::unique_ptr<FMeshGeometry>> Geometries;
	std::unordered_map<std::string, std::unique_ptr<FMaterial>> Materials;
	std::unordered_map<std::string, std::unique_ptr<FTexture>> Textures;

	std::unordered_map<std::string, ComPtr<ID3DBlob>> Shaders;

	std::vector<D3D12_INPUT_ELEMENT_DESC> InputLayout;


	ComPtr<ID3D12PipelineState> OpaquePSO = nullptr;

	// List of all the render items.
	std::vector<std::unique_ptr<FRenderItem>> AllRenderItems;

	// Render items divided by PSO.
	std::vector<FRenderItem *> OpaqueRitems;

	FPassConstants MainPassCB;
	UINT PassCBVOffset = 0;

	bool bIsWireFrame = false;

	XMFLOAT3 EyePos = {0.0f, 0.0f, 0.0f};
	XMFLOAT4X4 View = MathHelper::Identity4x4();
	XMFLOAT4X4 Proj = MathHelper::Identity4x4();

	float Theta = 1.5f * XM_PI;
	float Phi = XM_PIDIV4;
	float Radius = 15.0f;

	POINT LastMousePos;
};

FLitColumn::FLitColumn(HINSTANCE hInstance)
	: FD3DApp(hInstance)
{
}

FLitColumn::~FLitColumn()
{
}

bool FLitColumn::Initialize()
{
	if (!FD3DApp::Initialize())
	{
		return false;
	}

	// Reset the command list to prep for initialization commands.
	ThrowIfFailed(
		CommandList->Reset(DirectCmdListAlloctor.Get(), nullptr));

	BuildRootSignature();
	BuildShadersAndInputLayout();
	BuildShapeGeometry();
	BuildSkullGeometry();
	BuildMaterials();
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

void FLitColumn::OnResize()
{
	FD3DApp::OnResize();

	// The window resized, so update the aspect ratio and recompute the projection matrix.
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	DirectX::XMStoreFloat4x4(&Proj, P);
}

void FLitColumn::Update(const GameTimer &gt)
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
	UpdateMaterialCBs(gt);
}

void FLitColumn::Draw(const GameTimer &gt)
{
	auto CmdListAlloc = CurrentFrameResource->CommandListAllocator;

	// Reuse the memory associated with command recording.
	// We can only reset when the associated command lists have finished execution on th
	ThrowIfFailed(CmdListAlloc->Reset());

	// A command list can be reset after it has been added to the command queue via ExecuteCommandList.
	// Reusing the command list reuses memory.

	ThrowIfFailed(CommandList->Reset(CmdListAlloc.Get(), OpaquePSO.Get()));

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
	auto PassCB = CurrentFrameResource->PassCB->Resource();
	CommandList->SetGraphicsRootConstantBufferView(2, PassCB->GetGPUVirtualAddress());
	
	DrawRenderItems(CommandList.Get(), OpaqueRitems);

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

void FLitColumn::OnMouseDown(WPARAM btnState, int x, int y)
{
	LastMousePos.x = x;
	LastMousePos.y = y;

	SetCapture(MainWnd);
}

void FLitColumn::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void FLitColumn::OnMouseMove(WPARAM btnState, int x, int y)
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

void FLitColumn::OnKeyboardInput(const GameTimer &gt)
{
	if (GetAsyncKeyState('1') & 0x8000)
		bIsWireFrame = true;
	else
		bIsWireFrame = false;
}

void FLitColumn::UpdateCamera(const GameTimer &gt)
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

void FLitColumn::UpdateObjectCBs(const GameTimer &gt)
{
	auto CurrentObjectCB = CurrentFrameResource->ObjectCB.get();
	for (auto &Item : AllRenderItems)
	{
		if (Item->NumFrameDirty > 0)
		{
			XMMATRIX World = XMLoadFloat4x4(&Item->World);
			XMMATRIX TexTransform = XMLoadFloat4x4(&Item->TexTransform);

			FObjectConstants ObjConstants;
			XMStoreFloat4x4(&ObjConstants.World, XMMatrixTranspose(World));
			XMStoreFloat4x4(&ObjConstants.TexTransform, XMMatrixTranspose(TexTransform));

			CurrentObjectCB->CopyData(Item->ObjCBIndex, ObjConstants);

			Item->NumFrameDirty--;
		}
	}
}

void FLitColumn::UpdateMaterialCBs(const GameTimer &gt)
{
	auto CurrentMaterialCB = CurrentFrameResource->MaterialCB.get();
	for (auto &E : Materials)
	{
		// Only update the cbuffer data if the constants have changed.  If the cbuffer
		// data changes, it needs to be updated for each FrameResource.
		FMaterial *Mat = E.second.get();
		if (Mat->NumFramesDirty > 0)
		{
			XMMATRIX TransForm = XMLoadFloat4x4(&Mat->MatTransform);

			FMaterialConstants MaterialConstants;
			MaterialConstants.DiffuseAlbedo = Mat->DiffuseAlbedo;
			MaterialConstants.FresnelR0 = Mat->FresnelR0;
			MaterialConstants.Roughness = Mat->Roughness;

			XMStoreFloat4x4(&MaterialConstants.MatTransform, XMMatrixTranspose(TransForm));

			CurrentMaterialCB->CopyData(Mat->MatCBIndex, MaterialConstants);

			--Mat->NumFramesDirty;
		}
	}
}

void FLitColumn::UpdateMainPassCB(const GameTimer &gt)
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

	MainPassCB.AmbientLight = {0.25f, 0.25f, 0.35f, 1.0f};
	MainPassCB.Lights[0].Direction = {0.57735f, -0.57735f, 0.57735f};
	MainPassCB.Lights[0].Strength = {0.6f, 0.6f, 0.6f};
	MainPassCB.Lights[1].Direction = {-0.57735f, -0.57735f, 0.57735f};
	MainPassCB.Lights[1].Strength = {0.3f, 0.3f, 0.3f};
	MainPassCB.Lights[2].Direction = {0.0f, -0.707f, -0.707f};
	MainPassCB.Lights[2].Strength = {0.15f, 0.15f, 0.15f};

	auto CurPassCB = CurrentFrameResource->PassCB.get();
	CurPassCB->CopyData(0, MainPassCB);
}

void FLitColumn::BuildRootSignature()
{
	CD3DX12_ROOT_PARAMETER SlotRootParameter[3];

	SlotRootParameter[0].InitAsConstantBufferView(0);
	SlotRootParameter[1].InitAsConstantBufferView(1);
	SlotRootParameter[2].InitAsConstantBufferView(2);

	// A root signature is an array of root parameters.
	CD3DX12_ROOT_SIGNATURE_DESC RootSigDesc(3, SlotRootParameter, 0, nullptr,
											D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

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

void FLitColumn::BuildMaterials()
{
	auto bricks0 = std::make_unique<FMaterial>();
	bricks0->Name = "bricks0";
	bricks0->MatCBIndex = 0;
	bricks0->DiffuseSrvHeapIndex = 0;
	bricks0->DiffuseAlbedo = XMFLOAT4(Colors::ForestGreen);
	bricks0->FresnelR0 = XMFLOAT3(0.02f, 0.02f, 0.02f);
	bricks0->Roughness = 0.1f;

	auto stone0 = std::make_unique<FMaterial>();
	stone0->Name = "stone0";
	stone0->MatCBIndex = 1;
	stone0->DiffuseSrvHeapIndex = 1;
	stone0->DiffuseAlbedo = XMFLOAT4(Colors::LightSteelBlue);
	stone0->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
	stone0->Roughness = 0.3f;

	auto tile0 = std::make_unique<FMaterial>();
	tile0->Name = "tile0";
	tile0->MatCBIndex = 2;
	tile0->DiffuseSrvHeapIndex = 2;
	tile0->DiffuseAlbedo = XMFLOAT4(Colors::LightGray);
	tile0->FresnelR0 = XMFLOAT3(0.02f, 0.02f, 0.02f);
	tile0->Roughness = 0.2f;

	auto skullMat = std::make_unique<FMaterial>();
	skullMat->Name = "skullMat";
	skullMat->MatCBIndex = 3;
	skullMat->DiffuseSrvHeapIndex = 3;
	skullMat->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	skullMat->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05);
	skullMat->Roughness = 0.3f;

	Materials["bricks0"] = std::move(bricks0);
	Materials["stone0"] = std::move(stone0);
	Materials["tile0"] = std::move(tile0);
	Materials["skullMat"] = std::move(skullMat);
}

void FLitColumn::BuildShadersAndInputLayout()
{
	const D3D_SHADER_MACRO AlphaTestDefines[] =
		{
			"ALPHA_TEST", "1",
			NULL, NULL};

	Shaders["standardVS"] = d3dUtil::CompileShader(L"Shaders\\Default.hlsl", nullptr, "VS", "vs_5_1");
	Shaders["opaquePS"] = d3dUtil::CompileShader(L"Shaders\\Default.hlsl", nullptr, "PS", "ps_5_1");

	InputLayout =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		};
}

void FLitColumn::BuildShapeGeometry()
{
	FGeometryGenerator GeometryGenerator;
	FGeometryGenerator::FMeshData Box = GeometryGenerator.CreateBox(1.5f, 0.5f, 1.5f, 3);
	FGeometryGenerator::FMeshData Grid = GeometryGenerator.CreateGrid(20.0f, 30.0f, 60, 40);
	FGeometryGenerator::FMeshData Sphere = GeometryGenerator.CreateSphere(0.5f, 20, 20);
	FGeometryGenerator::FMeshData Cylinder = GeometryGenerator.CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20);

	//
	// We are concatenating all the geometry into one big vertex/index buffer.  So
	// define the regions in the buffer each submesh covers.
	//

	// Cache the vertex offsets to each object in the concatenated vertex buffer.
	UINT BoxVertexOffset = 0;
	UINT GridVertexOffset = (UINT)Box.Vertices.size();
	UINT SphereVertexOffset = GridVertexOffset + (UINT)Grid.Vertices.size();
	UINT CylinderVertexOffset = SphereVertexOffset + (UINT)Sphere.Vertices.size();

	// Cache the starting index for each object in the concatenated index buffer.
	UINT BoxIndexOffset = 0;
	UINT GridIndexOffset = (UINT)Box.Indices32.size();
	UINT SphereIndexOffset = GridIndexOffset + (UINT)Grid.Indices32.size();
	UINT CylinderIndexOffset = SphereIndexOffset + (UINT)Sphere.Indices32.size();

	// Define the SubmeshGeometry that cover different
	// regions of the vertex/index buffers.
	FSubmeshGeometry BoxSubmesh;
	BoxSubmesh.IndexCount = (UINT)Box.Indices32.size();
	BoxSubmesh.StartIndexLocation = BoxIndexOffset;
	BoxSubmesh.BaseVertexLocation = BoxVertexOffset;

	FSubmeshGeometry GridSubmesh;
	GridSubmesh.IndexCount = (UINT)Grid.Indices32.size();
	GridSubmesh.StartIndexLocation = GridIndexOffset;
	GridSubmesh.BaseVertexLocation = GridVertexOffset;

	FSubmeshGeometry SphereSubmesh;
	SphereSubmesh.IndexCount = (UINT)Sphere.Indices32.size();
	SphereSubmesh.StartIndexLocation = SphereIndexOffset;
	SphereSubmesh.BaseVertexLocation = SphereVertexOffset;

	FSubmeshGeometry CylinderSubmesh;
	CylinderSubmesh.IndexCount = (UINT)Cylinder.Indices32.size();
	CylinderSubmesh.StartIndexLocation = CylinderIndexOffset;
	CylinderSubmesh.BaseVertexLocation = CylinderVertexOffset;

	//
	// Extract the vertex elements we are interested in and pack the
	// vertices of all the meshes into one vertex buffer.
	//
	UINT TotalVertexCount = Box.Vertices.size() + Grid.Vertices.size() + Sphere.Vertices.size() + Cylinder.Vertices.size();

	std::vector<FVertex> Vertices(TotalVertexCount);

	size_t Index = 0;
	for (size_t i = 0; i < Box.Vertices.size(); ++i, ++Index)
	{
		Vertices[Index].Pos = Box.Vertices[i].Position;
		Vertices[Index].Normal = Box.Vertices[i].Normal;
	}

	for (size_t i = 0; i < Grid.Vertices.size(); ++i, ++Index)
	{
		Vertices[Index].Pos = Grid.Vertices[i].Position;
		Vertices[Index].Normal = Grid.Vertices[i].Normal;
	}

	for (size_t i = 0; i < Sphere.Vertices.size(); ++i, ++Index)
	{
		Vertices[Index].Pos = Sphere.Vertices[i].Position;
		Vertices[Index].Normal = Sphere.Vertices[i].Normal;
	}

	for (size_t i = 0; i < Cylinder.Vertices.size(); ++i, ++Index)
	{
		Vertices[Index].Pos = Cylinder.Vertices[i].Position;
		Vertices[Index].Normal = Cylinder.Vertices[i].Normal;
	}

	std::vector<std::uint16_t> Indices;
	Indices.insert(Indices.end(), std::begin(Box.GetIndices16()), std::end(Box.GetIndices16()));
	Indices.insert(Indices.end(), std::begin(Grid.GetIndices16()), std::end(Grid.GetIndices16()));
	Indices.insert(Indices.end(), std::begin(Sphere.GetIndices16()), std::end(Sphere.GetIndices16()));
	Indices.insert(Indices.end(), std::begin(Cylinder.GetIndices16()), std::end(Cylinder.GetIndices16()));

	const UINT VBByteSize = (UINT)Vertices.size() * sizeof(FVertex);
	const UINT IBByteSize = (UINT)Indices.size() * sizeof(std::uint16_t);

	auto Geo = std::make_unique<FMeshGeometry>();
	Geo->Name = ITEM_SHAPE;

	ThrowIfFailed(D3DCreateBlob(VBByteSize, &Geo->VertexBufferCPU));
	CopyMemory(Geo->VertexBufferCPU->GetBufferPointer(), Vertices.data(), VBByteSize);

	ThrowIfFailed(D3DCreateBlob(IBByteSize, &Geo->IndexBufferCPU));
	CopyMemory(Geo->IndexBufferCPU->GetBufferPointer(), Indices.data(), IBByteSize);

	Geo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(D3DDevice.Get(), CommandList.Get(), Vertices.data(), VBByteSize, Geo->VertexBufferUploader);
	Geo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(D3DDevice.Get(), CommandList.Get(), Indices.data(), IBByteSize, Geo->IndexBufferUploader);

	Geo->VertexByteStride = sizeof(FVertex);
	Geo->VertexBufferByteSize = VBByteSize;
	Geo->IndexBufferByteSize = DXGI_FORMAT_R16_UINT;
	Geo->IndexBufferByteSize = IBByteSize;

	Geo->DrawArgs[ITEM_BOX] = BoxSubmesh;
	Geo->DrawArgs[ITEM_GRID] = GridSubmesh;
	Geo->DrawArgs[ITEM_SPHERE] = SphereSubmesh;
	Geo->DrawArgs[ITEM_CYLINDER] = CylinderSubmesh;

	Geometries[Geo->Name] = std::move(Geo);
}

void FLitColumn::BuildSkullGeometry()
{
	std::wstring PathName = d3dUtil::GetPath(L"Models/skull.txt");
	std::ifstream fin(PathName.c_str());
	if (!fin)
	{
		MessageBox(0, L"Models/skull.txt not found.", 0, 0);
		return;
	}
	UINT VCount = 0;
	UINT TCount = 0;
	std::string Ignore;

	fin >> Ignore >> VCount;
	fin >> Ignore >> TCount;
	fin >> Ignore >> Ignore >> Ignore >> Ignore;

	std::vector<FVertex> Vertices(VCount);
	for (UINT i = 0; i < VCount; ++i)
	{
		fin >> Vertices[i].Pos.x >> Vertices[i].Pos.y >> Vertices[i].Pos.z;
		fin >> Vertices[i].Normal.x >> Vertices[i].Normal.y >> Vertices[i].Normal.z;
	}

	fin >> Ignore;
	fin >> Ignore;
	fin >> Ignore;

	std::vector<std::int32_t> Indices(3 * TCount);
	for (UINT i = 0; i < TCount; ++i)
	{
		fin >> Indices[i * 3 + 0] >> Indices[i * 3 + 1] >> Indices[i * 3 + 2];
	}

	fin.close();

	//
	// Pack the indices of all the meshes into one index buffer.
	//

	const UINT VBByteSize = (UINT)Vertices.size() * sizeof(FVertex);
	const UINT IBByteSize = (UINT)Indices.size() * sizeof(std::int32_t);

	auto Geo = std::make_unique<FMeshGeometry>();
	Geo->Name = "skullGeo";

	ThrowIfFailed(D3DCreateBlob(VBByteSize, &Geo->VertexBufferCPU));
	CopyMemory(Geo->VertexBufferCPU->GetBufferPointer(), Vertices.data(), VBByteSize);

	ThrowIfFailed(D3DCreateBlob(IBByteSize, &Geo->IndexBufferCPU));
	CopyMemory(Geo->IndexBufferCPU->GetBufferPointer(), Indices.data(), IBByteSize);

	Geo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(D3DDevice.Get(), CommandList.Get(), Vertices.data(), VBByteSize, Geo->VertexBufferUploader);
	Geo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(D3DDevice.Get(), CommandList.Get(), Indices.data(), VBByteSize, Geo->IndexBufferUploader);

	Geo->VertexByteStride = sizeof(FVertex);
	Geo->VertexBufferByteSize = VBByteSize;
	Geo->IndexFormat = DXGI_FORMAT_R32_UINT;
	Geo->IndexBufferByteSize = IBByteSize;

	FSubmeshGeometry SubMesh;
	SubMesh.IndexCount = (UINT)Indices.size();
	SubMesh.StartIndexLocation = 0;
	SubMesh.BaseVertexLocation = 0;

	Geo->DrawArgs["skull"] = SubMesh;

	Geometries[Geo->Name] = std::move(Geo);
}

void FLitColumn::BuildRenderItems()
{
	auto BoxRenderItem = std::make_unique<FRenderItem>();
	XMStoreFloat4x4(&BoxRenderItem->World, XMMatrixScaling(2.0f, 2.0f, 2.0f) * XMMatrixTranslation(0.0f, 0.5f, 0.0f));
	BoxRenderItem->ObjCBIndex = 0;
	BoxRenderItem->Mat = Materials["stone0"].get();
	BoxRenderItem->Geo = Geometries[ITEM_SHAPE].get();
	BoxRenderItem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	BoxRenderItem->IndexCount = BoxRenderItem->Geo->DrawArgs[ITEM_BOX].IndexCount;
	BoxRenderItem->StartIndexLocation = BoxRenderItem->Geo->DrawArgs[ITEM_BOX].StartIndexLocation;
	BoxRenderItem->BaseVertexLocation = BoxRenderItem->Geo->DrawArgs[ITEM_BOX].BaseVertexLocation;
	AllRenderItems.push_back(std::move(BoxRenderItem));

	auto GridRenderItem = std::make_unique<FRenderItem>();
	GridRenderItem->World = MathHelper::Identity4x4();
	GridRenderItem->ObjCBIndex = 1;
	GridRenderItem->Mat = Materials["tile0"].get();
	GridRenderItem->Geo = Geometries[ITEM_SHAPE].get();
	GridRenderItem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	GridRenderItem->IndexCount = GridRenderItem->Geo->DrawArgs[ITEM_GRID].IndexCount;
	GridRenderItem->StartIndexLocation = GridRenderItem->Geo->DrawArgs[ITEM_GRID].StartIndexLocation;
	GridRenderItem->BaseVertexLocation = GridRenderItem->Geo->DrawArgs[ITEM_GRID].BaseVertexLocation;
	AllRenderItems.push_back(std::move(GridRenderItem));


	auto SkullRitem = std::make_unique<FRenderItem>();
	XMStoreFloat4x4(&SkullRitem->World, XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixTranslation(0.0f, 1.0f, 0.0f));
	SkullRitem->TexTransform = MathHelper::Identity4x4();
	SkullRitem->ObjCBIndex = 2;
	SkullRitem->Mat = Materials["skullMat"].get();
	SkullRitem->Geo = Geometries["skullGeo"].get();
	SkullRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	SkullRitem->IndexCount = SkullRitem->Geo->DrawArgs["skull"].IndexCount;
	SkullRitem->StartIndexLocation = SkullRitem->Geo->DrawArgs["skull"].StartIndexLocation;
	SkullRitem->BaseVertexLocation = SkullRitem->Geo->DrawArgs["skull"].BaseVertexLocation;
	AllRenderItems.push_back(std::move(SkullRitem));

	UINT ObjCBIndex = 3;
	for (int i = 0; i < 5; ++i)
	{
		auto LeftCylinderItem = std::make_unique<FRenderItem>();
		auto LeftSphereItem = std::make_unique<FRenderItem>();

		auto RightCylinderItem = std::make_unique<FRenderItem>();
		auto RightSphereItem = std::make_unique<FRenderItem>();

		XMMATRIX LeftCylinderlWorld = XMMatrixTranslation(-5.0f, 1.5f, -10.0f + i * 5.0f);
		XMMATRIX RightCylinderWorld = XMMatrixTranslation(+5.0f, 1.5f, -10.0f + i * 5.0f);

		XMMATRIX LeftSphereWorld = XMMatrixTranslation(-5.0f, 3.5f, -10.0f + i * 5.0f);
		XMMATRIX RightSphereWorld = XMMatrixTranslation(+5.0f, 3.5f, -10.0f + i * 5.0f);

		XMStoreFloat4x4(&LeftCylinderItem->World, LeftCylinderlWorld);
		LeftCylinderItem->ObjCBIndex = ObjCBIndex++;
		LeftCylinderItem->Mat = Materials["bricks0"].get();
		LeftCylinderItem->Geo = Geometries[ITEM_SHAPE].get();
		LeftCylinderItem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		LeftCylinderItem->IndexCount = LeftCylinderItem->Geo->DrawArgs[ITEM_CYLINDER].IndexCount;
		LeftCylinderItem->StartIndexLocation = LeftCylinderItem->Geo->DrawArgs[ITEM_CYLINDER].StartIndexLocation;
		LeftCylinderItem->BaseVertexLocation = LeftCylinderItem->Geo->DrawArgs[ITEM_CYLINDER].BaseVertexLocation;

		XMStoreFloat4x4(&LeftSphereItem->World, LeftSphereWorld);
		LeftSphereItem->ObjCBIndex = ObjCBIndex++;
		LeftSphereItem->Mat = Materials["bricks0"].get();
		LeftSphereItem->Geo = Geometries[ITEM_SHAPE].get();
		LeftSphereItem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		LeftSphereItem->IndexCount = LeftSphereItem->Geo->DrawArgs[ITEM_SPHERE].IndexCount;
		LeftSphereItem->StartIndexLocation = LeftSphereItem->Geo->DrawArgs[ITEM_SPHERE].StartIndexLocation;
		LeftSphereItem->BaseVertexLocation = LeftSphereItem->Geo->DrawArgs[ITEM_SPHERE].BaseVertexLocation;

		XMStoreFloat4x4(&RightCylinderItem->World, RightCylinderWorld);
		RightCylinderItem->ObjCBIndex = ObjCBIndex++;
		RightCylinderItem->Mat = Materials["bricks0"].get();
		RightCylinderItem->Geo = Geometries[ITEM_SHAPE].get();
		RightCylinderItem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		RightCylinderItem->IndexCount = RightCylinderItem->Geo->DrawArgs[ITEM_CYLINDER].IndexCount;
		RightCylinderItem->StartIndexLocation = RightCylinderItem->Geo->DrawArgs[ITEM_CYLINDER].StartIndexLocation;
		RightCylinderItem->BaseVertexLocation = RightCylinderItem->Geo->DrawArgs[ITEM_CYLINDER].BaseVertexLocation;

		XMStoreFloat4x4(&RightSphereItem->World, RightSphereWorld);
		RightSphereItem->ObjCBIndex = ObjCBIndex++;
		RightSphereItem->Mat = Materials["bricks0"].get();
		RightSphereItem->Geo = Geometries[ITEM_SHAPE].get();
		RightSphereItem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		RightSphereItem->IndexCount = RightSphereItem->Geo->DrawArgs[ITEM_SPHERE].IndexCount;
		RightSphereItem->StartIndexLocation = RightSphereItem->Geo->DrawArgs[ITEM_SPHERE].StartIndexLocation;
		RightSphereItem->BaseVertexLocation = RightSphereItem->Geo->DrawArgs[ITEM_SPHERE].BaseVertexLocation;

		AllRenderItems.push_back(std::move(LeftCylinderItem));
		AllRenderItems.push_back(std::move(RightCylinderItem));
		AllRenderItems.push_back(std::move(LeftSphereItem));
		AllRenderItems.push_back(std::move(RightSphereItem));
	}

	for (auto &Item : AllRenderItems)
	{
		OpaqueRitems.push_back(Item.get());
	}
}
void FLitColumn::DrawRenderItems(ID3D12GraphicsCommandList *CommandList, const std::vector<FRenderItem *> &RenderItems)
{
	UINT ObjCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(FObjectConstants));
	UINT MatCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(FMaterialConstants));

	auto ObjectCB = CurrentFrameResource->ObjectCB->Resource();
	auto MatCB = CurrentFrameResource->MaterialCB->Resource();

	// For each render item...
	for (size_t i = 0; i < RenderItems.size(); ++i)
	{
		FRenderItem *Item = RenderItems[i];

		CommandList->IASetVertexBuffers(0, 1, &Item->Geo->VertexBufferView());
		CommandList->IASetIndexBuffer(&Item->Geo->IndexBufferView());
		CommandList->IASetPrimitiveTopology(Item->PrimitiveType);

		D3D12_GPU_VIRTUAL_ADDRESS ObjCBAddress = ObjectCB->GetGPUVirtualAddress() + Item->ObjCBIndex * ObjCBByteSize;
		D3D12_GPU_VIRTUAL_ADDRESS MatCBAddress = MatCB->GetGPUVirtualAddress() + Item->Mat->MatCBIndex * MatCBByteSize;
		
		CommandList->SetGraphicsRootConstantBufferView(0, ObjCBAddress);
		CommandList->SetGraphicsRootConstantBufferView(1, MatCBAddress);
		
		CommandList->DrawIndexedInstanced(Item->IndexCount, 1, Item->StartIndexLocation, Item->BaseVertexLocation, 0);
	}
}

void FLitColumn::BuildFrameResources()
{
	for (int i = 0; i < gNumFrameResources; ++i)
	{
		FrameResources.push_back(
			std::make_unique<FFrameResource>(
				D3DDevice.Get(),
				1,
				(UINT)AllRenderItems.size(),
				(UINT)Materials.size()));
	}
}

void FLitColumn::BuildPSOs()
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
			Shaders["standardVS"]->GetBufferSize()};
	OpaquePSODesc.PS =
		{
			reinterpret_cast<BYTE *>(Shaders["opaquePS"]->GetBufferPointer()),
			Shaders["opaquePS"]->GetBufferSize()};
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
		D3DDevice->CreateGraphicsPipelineState(&OpaquePSODesc, IID_PPV_ARGS(&OpaquePSO)));
}

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	try
	{
		FLitColumn App(hInst);
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
