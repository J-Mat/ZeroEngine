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
	XMFLOAT4X4 WorldViewProj = MathHelper::Identity4x4();
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

	ComPtr<ID3D12RootSignature> RootSignature = nullptr;
	ComPtr<ID3D12DescriptorHeap> CBVHeap = nullptr;

	ComPtr<ID3D12DescriptorHeap> mSrvDescriptorHeap = nullptr;

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
	// Convert Spherical to Cartesian coordinates.
	float X = Radius * sinf(Phi) * cosf(Theta);
	float Z = Radius * sinf(Phi) * sinf(Theta);
	float Y = Radius * cosf(Phi);
	
	XMVECTOR Pos = XMVectorSet( X, Y, Z, 1.0f );
	XMVECTOR Target = XMVectorZero();
	XMVECTOR Up = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
		
	XMMATRIX ViewMatrix = DirectX::XMMatrixLookAtLH(Pos, Target, Up);
	XMStoreFloat4x4(&View, ViewMatrix);
	
	XMMATRIX WorldMatrix = XMLoadFloat4x4(&World);
	XMMATRIX ProjMatrix = XMLoadFloat4x4(&Proj);
	XMMATRIX WorldViewProj = WorldMatrix * ViewMatrix * ProjMatrix;

	// Update the constant buffer with the latest worldViewProj matrix.
	FObjectConstants ObjConstants;
	XMStoreFloat4x4(&ObjConstants.WorldViewProj, XMMatrixTranspose(WorldViewProj));
	ObjectCB->CopyData(0, ObjConstants);
}

void FShapeApp::Draw(const GameTimer& gt)
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

void FShapeApp::BuildDescriptorHeaps()
{
	//UINT ObjCount
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
	Shaders["StandarVS"] = d3dUtil::CompileShader(L"Shaders\\Color.hlsl", nullptr, "VS", "vs_5_1");
	Shaders["OpaquePS"] = d3dUtil::CompileShader(L"Shaders\\color.hlsl", nullptr, "PS", "ps_5_1");
	
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
	Geo->Name = "ShapGeo";
	
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
	//xxoo
}

void FShapeApp::BuildPSOs()
{
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
