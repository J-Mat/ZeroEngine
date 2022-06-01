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

using namespace DirectX;
using namespace Microsoft::WRL;
//using namespace DirectX::PackedVector;

const int gNumFrameResources = 3;


#define ITEM_SHAPE    "Shape"
#define ITEM_BOX      "Box"
#define ITEM_GRID     "Grid"
#define ITEM_SPHERE   "Sphere"
#define ITEM_CYLINDER "Cylinder"


struct FVertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

struct FObjectConstants
{
	XMFLOAT4X4 World = MathHelper::Identity4x4();
};

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
	
	MeshGeometry* Geo = nullptr;
	
	// Primitive topology.
	D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// DrawIndexedInstanced parameters.
	UINT IndexCount = 0;
	UINT StartIndexLocation = 0;
	int BaseVertexLocation = 0;
};

class FShapeApp : public FD3DApp
{
public:
	FShapeApp(HINSTANCE hInstance);
	~FShapeApp();

	virtual bool Initialize() override;

private:
	virtual void OnResize()override;
	virtual void Update(const GameTimer& gt)override;
	virtual void Draw(const GameTimer& gt)override;

	virtual void OnMouseDown(WPARAM btnState, int x, int y)override;
	virtual void OnMouseUp(WPARAM btnState, int x, int y)override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y)override;

	void OnKeyboardInput(const GameTimer& gt);
	void UpdateCamera(const GameTimer& gt);
	void UpdateObjectCBs(const GameTimer& gt);
	void UpdateMainPassCB(const GameTimer& gt);

    void BuildDescriptorHeaps();
    void BuildConstantBufferViews();
    void BuildRootSignature();
    void BuildShadersAndInputLayout();
    void BuildShapeGeometry();
    void BuildPSOs();
    void BuildFrameResources();
    void BuildRenderItems();
    void DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<FRenderItem*>& ritems);
private:
	std::vector<std::unique_ptr<FFrameResource>> FrameResources;
	FFrameResource* CurrentFrameResource = nullptr;
	int CurrentFrameResourceIndex = 0;

	ComPtr<ID3D12RootSignature> RootSignature = nullptr;
	ComPtr<ID3D12DescriptorHeap> CBVHeap = nullptr;

	ComPtr<ID3D12DescriptorHeap> SrvDescriptorHeap = nullptr;

	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> Geometries;
	std::unordered_map<std::string, ComPtr<ID3DBlob>> Shaders;
	std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> PSOs;
	
	std::vector<D3D12_INPUT_ELEMENT_DESC> InputLayout;


	//List of all the render items.
	std::vector<std::unique_ptr<FRenderItem>> AllRenderItems;

	// Render items divided by PSO.
	std::vector<FRenderItem*> OpaqueRitems;

	FPassConstants MainPassCB;
	UINT PassCBVOffset = 0;
	
	bool bIsWireFrame = false;
	
	
	XMFLOAT3 EyePos = {0.0f, 0.0f, 0.0f};
	XMFLOAT4X4 View = MathHelper::Identity4x4();
	XMFLOAT4X4 Proj = MathHelper::Identity4x4();

	float Theta = 1.5f * XM_PI;
	float Phi = XM_PIDIV4;
	float Radius = 5.0f;

	POINT LastMousePos;
};


FShapeApp::FShapeApp(HINSTANCE hInstance)
	: FD3DApp(hInstance)
{

}

FShapeApp::~FShapeApp()
{

}

bool FShapeApp::Initialize()
{
	if (!FD3DApp::Initialize())
	{
		return false;
	}

	// Reset the command list to prep for initialization commands.
	ThrowIfFailed(
		CommandList->Reset(DirectCmdListAlloctor.Get(), nullptr)
	);
	
	BuildRootSignature();
	BuildShadersAndInputLayout();
	BuildShapeGeometry();
	BuildRenderItems();
	BuildFrameResources();
	BuildDescriptorHeaps();
	BuildConstantBufferViews();
	BuildPSOs();

	// Execute the initialization commands.
	ThrowIfFailed(CommandList->Close());
	ID3D12CommandList* CmdsLists[] = { CommandList.Get() };
	CommandQueue->ExecuteCommandLists(_countof(CmdsLists), CmdsLists);

	// Wait until initialization is complete.
	FlushCommandQueue();

	return true;
}

void FShapeApp::OnResize()
{
	FD3DApp::OnResize();

	// The window resized, so update the aspect ratio and recompute the projection matrix.
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	DirectX::XMStoreFloat4x4(&Proj, P);
}

void FShapeApp::Update(const GameTimer& gt)
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
		HANDLE EventHandle = CreateEventEx(nullptr, false , false, EVENT_ALL_ACCESS);
		ThrowIfFailed(Fence->SetEventOnCompletion(CurrentFrameResource->Fence, EventHandle));
		WaitForSingleObject(EventHandle, INFINITE);
		CloseHandle(EventHandle);
	}
	
	UpdateObjectCBs(gt);
	UpdateMainPassCB(gt);
	
}

void FShapeApp::Draw(const GameTimer& gt)
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

	CommandList->ResourceBarrier(1, & CD3DX12_RESOURCE_BARRIER::Transition(
		GetCurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET)
	);
	
	// Clear the back buffer and depth buffer.
	CommandList->ClearRenderTargetView(GetCurrentBackBufferView(), Colors::AliceBlue, 0, nullptr);
	CommandList->ClearDepthStencilView(GetDepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

   	// Specify the buffers we are going to render to.
	CommandList->OMSetRenderTargets(1, &GetCurrentBackBufferView(), true, &GetDepthStencilView());

	ID3D12DescriptorHeap* DescriptorHeaps[] = {CBVHeap.Get()};
	CommandList->SetDescriptorHeaps(_countof(DescriptorHeaps), DescriptorHeaps);
	
	CommandList->SetGraphicsRootSignature(RootSignature.Get());
	
	int PassCBVIndex = PassCBVOffset + CurrentFrameResourceIndex;
	auto PassCBVHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(CBVHeap->GetGPUDescriptorHandleForHeapStart());
	PassCBVHandle.Offset(PassCBVIndex, CbvSrvUavDescriptorSize);
	CommandList->SetGraphicsRootDescriptorTable(1, PassCBVHandle);
	
	DrawRenderItems(CommandList.Get(), OpaqueRitems);
	
	// Indicate a state transition on the resource usage.
	CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		GetCurrentBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT)
	);
	
	// Done recording commands.
	ThrowIfFailed(CommandList->Close());

	// Add the command list to the queue for execution. 
	ID3D12CommandList* CmdsLists = { CommandList.Get()};
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



void FShapeApp::OnMouseDown(WPARAM btnState, int x, int y)
{
	LastMousePos.x = x;
	LastMousePos.y = y;
	
	SetCapture(MainWnd);
}

void FShapeApp::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void FShapeApp::OnMouseMove(WPARAM btnState, int x, int y)
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

void FShapeApp::OnKeyboardInput(const GameTimer& gt)
{
    if(GetAsyncKeyState('1') & 0x8000)
        bIsWireFrame = true;
    else
        bIsWireFrame = false;
}

void FShapeApp::UpdateCamera(const GameTimer& gt)
{
	// Convert Spherical to Cartesian coordinates.
	EyePos.x = Radius*sinf(Phi)*cosf(Theta);
	EyePos.z = Radius*sinf(Phi)*sinf(Theta);
	EyePos.y = Radius*cosf(Phi);

	// Build the view matrix.
	XMVECTOR pos = XMVectorSet(EyePos.x, EyePos.y, EyePos.z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&View, view);
}


void FShapeApp::UpdateObjectCBs(const GameTimer& gt)
{
	auto CurrentObjectCB = CurrentFrameResource->ObjectCB.get();
	for (auto& Item : AllRenderItems)
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

void FShapeApp::UpdateMainPassCB(const GameTimer& gt)
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

void FShapeApp::BuildDescriptorHeaps()
{
	UINT ObjCount = (UINT)OpaqueRitems.size();
	
   	// Need a CBV descriptor for each object for each frame resource,
    // +1 for the perPass CBV for each frame resource.
	UINT NumDescriptors = (ObjCount + 1) * gNumFrameResources;

	// Save an offset to the start of the pass CBVs.  These are the last 3 descriptors.
	PassCBVOffset = ObjCount * gNumFrameResources;

	D3D12_DESCRIPTOR_HEAP_DESC CBVHeapDesc;
	CBVHeapDesc.NumDescriptors = NumDescriptors;
	CBVHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	CBVHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	CBVHeapDesc.NodeMask = 0;
	ThrowIfFailed(
		D3DDevice->CreateDescriptorHeap(&CBVHeapDesc, IID_PPV_ARGS(&CBVHeap))
	);
}

void FShapeApp::BuildConstantBufferViews()
{
	UINT ObjCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(FObjectConstants));
	
	UINT ObjCount = (UINT)OpaqueRitems.size();
	
	// Need a CBV descriptor for each object for each frame resource.
	for (int FrameIndex = 0; FrameIndex < gNumFrameResources; ++FrameIndex)
	{
		auto ObjectCB = FrameResources[FrameIndex]->ObjectCB->Resource();
		for (UINT i = 0; i < ObjCount; ++i)
		{
			D3D12_GPU_VIRTUAL_ADDRESS CBAddress = ObjectCB->GetGPUVirtualAddress();
			
			// Offset to the ith object constant buffer in the buffer.
			CBAddress += i * ObjCBByteSize;
			
			 // Offset to the object cbv in the descriptor heap.
			int HeapIndex = FrameIndex * ObjCount + i; 
			auto Handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(CBVHeap->GetCPUDescriptorHandleForHeapStart());
			Handle.Offset(HeapIndex, CbvSrvUavDescriptorSize);
			
			D3D12_CONSTANT_BUFFER_VIEW_DESC CBVDesc;
			CBVDesc.BufferLocation = CBAddress;
			CBVDesc.SizeInBytes = ObjCBByteSize;
			
			D3DDevice->CreateConstantBufferView(&CBVDesc, Handle);
		}
	}
	
	UINT PassCByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(FPassConstants));

	// Last three descriptors are the pass CBVs for each frame resource.
	for (int FrameIndex = 0; FrameIndex < gNumFrameResources; ++FrameIndex)
	{
		auto PassCB = FrameResources[FrameIndex]->PassCB->Resource();
		D3D12_GPU_VIRTUAL_ADDRESS CBAddress = PassCB->GetGPUVirtualAddress();
		
		// Offset to the pass cbv in the descriptor heap.
		int HeapIndex = PassCBVOffset + FrameIndex;
		auto Handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(CBVHeap->GetCPUDescriptorHandleForHeapStart());
		Handle.Offset(HeapIndex, CbvSrvUavDescriptorSize);
			
		D3D12_CONSTANT_BUFFER_VIEW_DESC CBVDesc;
		CBVDesc.BufferLocation = CBAddress;
		CBVDesc.SizeInBytes = PassCByteSize;
			
		D3DDevice->CreateConstantBufferView(&CBVDesc, Handle);
	}
}

void FShapeApp::BuildRootSignature()
{
	CD3DX12_DESCRIPTOR_RANGE CBVTable0;
	CBVTable0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
	
	CD3DX12_DESCRIPTOR_RANGE CBVTable1;
	CBVTable0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
	
	// Root parameter can be a table, root descriptor or root constants.
	CD3DX12_ROOT_PARAMETER SlotRootParameter[2];
	
	// Create root CBVs.
	SlotRootParameter[0].InitAsDescriptorTable(1, &CBVTable0);
	SlotRootParameter[1].InitAsDescriptorTable(1, &CBVTable1);
	
	// A root signature is an array of root parameters.
	CD3DX12_ROOT_SIGNATURE_DESC RootSigDesc(2, SlotRootParameter, 0, nullptr, 
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	
	// create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
	ComPtr<ID3DBlob> SerializedRootSig = nullptr;
	ComPtr<ID3DBlob> ErrorBlob = nullptr;
	HRESULT HR  = D3D12SerializeRootSignature(&RootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, 
		SerializedRootSig.GetAddressOf(), ErrorBlob.GetAddressOf());
	
	if (ErrorBlob != nullptr)
	{
		::OutputDebugStringA((char*)ErrorBlob->GetBufferPointer());
	}
	ThrowIfFailed(HR);
	
	ThrowIfFailed(
		D3DDevice->CreateRootSignature(
			0,
			SerializedRootSig->GetBufferPointer(),
			SerializedRootSig->GetBufferSize(),
			IID_PPV_ARGS(RootSignature.GetAddressOf())
		)
	);	
}

void FShapeApp::BuildShadersAndInputLayout()
{
	HRESULT hr = S_OK;
	Shaders["standarVS"] = d3dUtil::CompileShader(L"Shaders\\Color.hlsl", nullptr, "VS", "vs_5_1");
	Shaders["opaquePS"] = d3dUtil::CompileShader(L"Shaders\\color.hlsl", nullptr, "PS", "ps_5_1");
	
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


void FShapeApp::BuildShapeGeometry()
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
	for (size_t i = 0;i < Box.Vertices.size(); ++i, ++Index)
	{
		Vertices[Index].Pos = Box.Vertices[i].Position;
		Vertices[Index].Color = XMFLOAT4(DirectX::Colors::DarkGreen);
	}

	for(size_t i = 0; i < Grid.Vertices.size(); ++i, ++Index)
	{
		Vertices[Index].Pos = Grid.Vertices[i].Position;
        Vertices[Index].Color = XMFLOAT4(DirectX::Colors::ForestGreen);
	}

	for(size_t i = 0; i < Sphere.Vertices.size(); ++i, ++Index)
	{
		Vertices[Index].Pos = Sphere.Vertices[i].Position;
        Vertices[Index].Color = XMFLOAT4(DirectX::Colors::Crimson);
	}

	for(size_t i = 0; i < Cylinder.Vertices.size(); ++i, ++Index)
	{
		Vertices[Index].Pos = Cylinder.Vertices[i].Position;
		Vertices[Index].Color = XMFLOAT4(DirectX::Colors::SteelBlue);
	}
	
	std::vector<std::uint16_t> Indices;
	Indices.insert(Indices.end(), std::begin(Box.GetIndices16()), std::end(Box.GetIndices16()));
	Indices.insert(Indices.end(), std::begin(Grid.GetIndices16()), std::end(Grid.GetIndices16()));
	Indices.insert(Indices.end(), std::begin(Sphere.GetIndices16()), std::end(Sphere.GetIndices16()));
	Indices.insert(Indices.end(), std::begin(Cylinder.GetIndices16()), std::end(Cylinder.GetIndices16()));
	
	const UINT VBByteSize = (UINT)Vertices.size() * sizeof(FVertex);
	const UINT IBByteSize = (UINT)Indices.size() * sizeof(std::uint16_t);
	
	auto Geo = std::make_unique<MeshGeometry>();
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

	Geo->DrawArgs["Box"] = BoxSubmesh;
	Geo->DrawArgs["Grid"] = GridSubmesh;
	Geo->DrawArgs["Sphere"] = SphereSubmesh;
	Geo->DrawArgs["Cylinder"] = CylinderSubmesh;
	
	Geometries[Geo->Name] = std::move(Geo);
}


void FShapeApp::BuildRenderItems()
{
	auto BoxRenderItem = std::make_unique<FRenderItem>();
	XMStoreFloat4x4(&BoxRenderItem->World, XMMatrixScaling(2.0f, 2.0f, 2.0f) * XMMatrixTranslation(0.0f, 0.5f, 0.0f));
	BoxRenderItem->ObjCBIndex = 0;
	BoxRenderItem->Geo = Geometries["ShapeGeo"].get();
	BoxRenderItem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	BoxRenderItem->IndexCount = BoxRenderItem->Geo->DrawArgs[ITEM_BOX].IndexCount;
	BoxRenderItem->StartIndexLocation = BoxRenderItem->Geo->DrawArgs[ITEM_BOX].StartIndexLocation;
	BoxRenderItem->BaseVertexLocation = BoxRenderItem->Geo->DrawArgs[ITEM_BOX].BaseVertexLocation;
	AllRenderItems.push_back(std::move(BoxRenderItem));
	
	auto GridRenderItem = std::make_unique<FRenderItem>();
	GridRenderItem->World = MathHelper::Identity4x4();
	GridRenderItem->ObjCBIndex = 1;
	GridRenderItem->Geo = Geometries["ShapeGeo"].get();
	GridRenderItem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	GridRenderItem->IndexCount = GridRenderItem->Geo->DrawArgs[ITEM_GRID].IndexCount;  
	GridRenderItem->StartIndexLocation = GridRenderItem->Geo->DrawArgs[ITEM_GRID].StartIndexLocation;
	GridRenderItem->BaseVertexLocation = GridRenderItem->Geo->DrawArgs[ITEM_GRID].BaseVertexLocation;
	AllRenderItems.push_back(std::move(GridRenderItem));

	UINT ObjCBIndex = 2;
	for (int i = 0; i < 5;++i)
	{
		auto LeftCylinderItem = std::make_unique<FRenderItem>();
		auto LeftSphereItem = std::make_unique<FRenderItem>();
		
		auto RightCylinderItem = std::make_unique<FRenderItem>();
		auto RightSphereItem = std::make_unique<FRenderItem>();
		
		XMMATRIX LeftCylinderlWorld = XMMatrixTranslation(-5.0f, 1.5f, -10.0f + i*5.0f);
		XMMATRIX RightCylinderWorld = XMMatrixTranslation(+5.0f, 1.5f, -10.0f + i*5.0f);

		XMMATRIX LeftSphereWorld = XMMatrixTranslation(-5.0f, 3.5f, -10.0f + i*5.0f);
		XMMATRIX RightSphereWorld = XMMatrixTranslation(+5.0f, 3.5f, -10.0f + i*5.0f);
		
		XMStoreFloat4x4(&LeftCylinderItem->World, LeftCylinderlWorld);
		LeftCylinderItem->ObjCBIndex = ObjCBIndex++;
		LeftCylinderItem->Geo = Geometries[ITEM_SHAPE].get();
		LeftCylinderItem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		LeftCylinderItem->IndexCount = LeftCylinderItem->Geo->DrawArgs[ITEM_CYLINDER].IndexCount;
		LeftCylinderItem->StartIndexLocation = LeftCylinderItem->Geo->DrawArgs[ITEM_CYLINDER].StartIndexLocation;
		LeftCylinderItem->BaseVertexLocation = LeftCylinderItem->Geo->DrawArgs[ITEM_CYLINDER].BaseVertexLocation;

		XMStoreFloat4x4(&LeftSphereItem->World, LeftSphereWorld);
		LeftSphereItem->ObjCBIndex = ObjCBIndex++;
		LeftSphereItem->Geo = Geometries[ITEM_SHAPE].get();
		LeftSphereItem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		LeftSphereItem->IndexCount = LeftSphereItem->Geo->DrawArgs[ITEM_SPHERE].IndexCount;
		LeftSphereItem->StartIndexLocation = LeftSphereItem->Geo->DrawArgs[ITEM_SPHERE].StartIndexLocation;
		LeftSphereItem->BaseVertexLocation = LeftSphereItem->Geo->DrawArgs[ITEM_SPHERE].BaseVertexLocation;

		XMStoreFloat4x4(&RightCylinderItem->World, RightCylinderWorld);
		RightCylinderItem->ObjCBIndex = ObjCBIndex++;
		RightCylinderItem->Geo = Geometries[ITEM_SHAPE].get();
		RightCylinderItem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		RightCylinderItem->IndexCount = RightCylinderItem->Geo->DrawArgs[ITEM_CYLINDER].IndexCount;
		RightCylinderItem->StartIndexLocation = RightCylinderItem->Geo->DrawArgs[ITEM_CYLINDER].StartIndexLocation;
		RightCylinderItem->BaseVertexLocation = RightCylinderItem->Geo->DrawArgs[ITEM_CYLINDER].BaseVertexLocation;

		XMStoreFloat4x4(&RightSphereItem->World, RightSphereWorld);
		RightSphereItem->ObjCBIndex = ObjCBIndex++;
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
	
	for (auto& Item : AllRenderItems)
	{
		OpaqueRitems.push_back(Item.get());
	}
}
void FShapeApp::DrawRenderItems(ID3D12GraphicsCommandList* CommandList, const std::vector<FRenderItem*>& RenderItems)
{
	UINT ObjCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(FObjectConstants));
	
	auto  ObjectCB = CurrentFrameResource->ObjectCB->Resource();
	
	// For each render item...
	for (size_t i = 0; i < RenderItems.size();++i)
	{
		FRenderItem* Item = RenderItems[i];
		
		CommandList->IASetVertexBuffers(0, 1, &Item->Geo->VertexBufferView());
		CommandList->IASetIndexBuffer(&Item->Geo->IndexBufferView());
		CommandList->IASetPrimitiveTopology(Item->PrimitiveType); 

		// Offset to the CBV in the descriptor heap for this object and for this frame
		UINT CBVIndex = CurrentFrameResourceIndex *(UINT)OpaqueRitems.size() + Item->ObjCBIndex;
		auto CBVHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(CBVHeap->GetGPUDescriptorHandleForHeapStart());
		CBVHandle.Offset(CBVIndex, CbvSrvUavDescriptorSize);
		
		CommandList->SetGraphicsRootDescriptorTable(0, CBVHandle);
		CommandList->DrawIndexedInstanced(Item->IndexCount, 1, Item->StartIndexLocation, Item->BaseVertexLocation, 0);
	}
}

void FShapeApp::BuildFrameResources()
{
	for (int i = 0;i < gNumFrameResources; ++i)
	{
		FrameResources.push_back(
			std::make_unique<FFrameResource>(
				D3DDevice.Get(), 
				1, 
				(UINT)AllRenderItems.size()
			)
		);
	}
}

void FShapeApp::BuildPSOs()
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
		reinterpret_cast<BYTE*>(Shaders["standardVS"]->GetBufferPointer()), 
		Shaders["standardVS"]->GetBufferSize()
	};
	OpaquePSODesc.PS = 
	{ 
		reinterpret_cast<BYTE*>(Shaders["opaquePS"]->GetBufferPointer()),
		Shaders["opaquePS"]->GetBufferSize()
	};
	OpaquePSODesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	OpaquePSODesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	OpaquePSODesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	OpaquePSODesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	OpaquePSODesc.SampleMask = UINT_MAX;
	OpaquePSODesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	OpaquePSODesc.NumRenderTargets = 1;
	OpaquePSODesc.RTVFormats[0] = BackBufferFormat;
	OpaquePSODesc.SampleDesc.Count = X4MSAAState ? 4: 1;
	OpaquePSODesc.SampleDesc.Quality = X4MSAAState ? (X4MSAAQuality - 1) : 0;
	OpaquePSODesc.DSVFormat = DepthStencilFormat;
	ThrowIfFailed(
		D3DDevice->CreateGraphicsPipelineState(&OpaquePSODesc, IID_PPV_ARGS(&PSOs["opaque"]))
	);

	//
    // PSO for opaque wireframe objects.
    //
	D3D12_GRAPHICS_PIPELINE_STATE_DESC opaqueWireframePsoDesc = OpaquePSODesc;
    opaqueWireframePsoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	ThrowIfFailed(
		D3DDevice->CreateGraphicsPipelineState(&opaqueWireframePsoDesc, IID_PPV_ARGS(&PSOs["opaque_wireframe"]))
	);
}


int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	
	try
	{
		FShapeApp App(hInst);
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
