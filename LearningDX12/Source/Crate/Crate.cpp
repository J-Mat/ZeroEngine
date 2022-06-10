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
using namespace DirectX;
using namespace Microsoft::WRL;
using namespace DirectX::PackedVector;

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
	FMeshGeometry* Geo = nullptr;

	// Primitive topology.
	D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// DrawIndexedInstanced parameters.
	UINT IndexCount = 0;
	UINT StartIndexLocation = 0;
	int BaseVertexLocation = 0;
};

struct FObjectConstants
{
	XMFLOAT4X4 World = MathHelper::Identity4x4();
};

class FCrateApp : public FD3DApp
{
public:
	FCrateApp(HINSTANCE hInstance);
	~FCrateApp();

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
	void UpdateObjectCBs(const GameTimer& gt)
	void UpdateMaterialCBs(const GameTimer& gt);
	void UpdateMainPassCB(const GameTimer& gt);

	void BuildFrameResources();
	void LoadTexture();
	void BuildDescriptorHeaps();
	void BuildRootSignature();
	void BuildShadersAndInputLayout();
	void BuildShapeGeometry();
	void BuildPSO();
	void BuildMaterials();
	void BuildRenderItems();
	void DrawRenderItems(ID3D12GraphicsCommandList* CommandList, const std::vector<FRenderItem*>& RenderItems);
	std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();

private:
	std::vector<std::unique_ptr<FFrameResource>> FrameResources;
	FFrameResource* CurrentFrameResource = nullptr;
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
	std::vector<FRenderItem*> OpaqueRitems;

	FPassConstants MainPassCB;
	UINT PassCBVOffset = 0;

	bool bIsWireFrame = false;

	XMFLOAT3 EyePos = { 0.0f, 0.0f, 0.0f };
	XMFLOAT4X4 View = MathHelper::Identity4x4();
	XMFLOAT4X4 Proj = MathHelper::Identity4x4();

	float Theta = 1.5f * XM_PI;
	float Phi = XM_PIDIV4;
	float Radius = 15.0f;

	POINT LastMousePos;
};


FCrateApp::FCrateApp(HINSTANCE hInstance)
	: FD3DApp(hInstance)
{

}

FCrateApp::~FCrateApp()
{

}

bool FCrateApp::Initialize()
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

void FCrateApp::OnResize()
{
	FD3DApp::OnResize();

	// The window resized, so update the aspect ratio and recompute the projection matrix.
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&Proj, P);
}

void FCrateApp::Update(const GameTimer& gt)
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

void FCrateApp::Draw(const GameTimer& gt)
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
	ID3D12CommandList* CmdsLists[] = { CommandList.Get() };
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



void FCrateApp::OnMouseDown(WPARAM btnState, int x, int y)
{
	LastMousePos.x = x;
	LastMousePos.y = y;
	
	SetCapture(MainWnd);
}

void FCrateApp::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void FCrateApp::OnMouseMove(WPARAM btnState, int x, int y)
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

void FCrateApp::OnKeyboardInput(const GameTimer& gt)
{

}

void FCrateApp::UpdateCamera(const GameTimer& gt)
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


void FCrateApp::UpdateMaterialCBs(const GameTimer& gt)
{
	auto CurrentMaterialCB = CurrentFrameResource->MaterialCB.get();
	for (auto& E : Materials)
	{
		// Only update the cbuffer data if the constants have changed.  If the cbuffer
		// data changes, it needs to be updated for each FrameResource.
		FMaterial* Mat = E.second.get();
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

void FCrateApp::UpdateMainPassCB(const GameTimer& gt)
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

	MainPassCB.AmbientLight = { 0.25f, 0.25f, 0.35f, 1.0f };
	MainPassCB.Lights[0].Direction = { 0.57735f, -0.57735f, 0.57735f };
	MainPassCB.Lights[0].Strength = { 0.6f, 0.6f, 0.6f };
	MainPassCB.Lights[1].Direction = { -0.57735f, -0.57735f, 0.57735f };
	MainPassCB.Lights[1].Strength = { 0.3f, 0.3f, 0.3f };
	MainPassCB.Lights[2].Direction = { 0.0f, -0.707f, -0.707f };
	MainPassCB.Lights[2].Strength = { 0.15f, 0.15f, 0.15f };

	auto CurPassCB = CurrentFrameResource->PassCB.get();
	CurPassCB->CopyData(0, MainPassCB);
}

void FCrateApp::BuildDescriptorHeaps()
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

	//
	// Fill out the heap with actual descriptors.
	//
	CD3DX12_CPU_DESCRIPTOR_HANDLE Descriptor(SrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
	
	auto WoodCrateTex = Textures["WoodCrateTex"]->Resource;
	
	D3D12_SHADER_RESOURCE_VIEW_DESC SrcDesc = {};
	SrcDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	SrcDesc.Format = WoodCrateTex->GetDesc().Format;
	SrcDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	SrcDesc.Texture2D.MostDetailedMip = 0;
	SrcDesc.Texture2D.MipLevels = WoodCrateTex->GetDesc().MipLevels;
	SrcDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	
	D3DDevice->CreateShaderResourceView(WoodCrateTex.Get(), &SrcDesc, Descriptor);
}

void FCrateApp::BuildFrameResources()
{
	for (int i = 0; i < gNumFrameResources; ++i)
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


void FCrateApp::LoadTexture()
{
	auto WoodCrateTex = std::make_unique<FTexture>();
	WoodCrateTex->Name = "WoodCrateTex";
	WoodCrateTex->Filename = d3dUtil::GetPath(L"Textures/WoodCrate01.dds");
	ThrowIfFailed(
		DirectX::CreateDDSTextureFromFile12(D3DDevice.Get(),
			CommandList.Get(),
			WoodCrateTex->Filename.c_str(),
			WoodCrateTex->Resource,
			WoodCrateTex->UploadHeap)
	);
	Textures[WoodCrateTex->Name] = std::move(WoodCrateTex);
}

void FCrateApp::BuildMaterials()
{
	auto woodCrate = std::make_unique<FMaterial>();
	woodCrate->Name = "woodCrate";
	woodCrate->MatCBIndex = 3;
	woodCrate->DiffuseSrvHeapIndex = 3;
	woodCrate->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	woodCrate->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05);
	woodCrate->Roughness = 0.3f;

	Materials["woodCrate"] = std::move(woodCrate);
}

void FCrateApp::BuildRenderItems()
{
}

void FCrateApp::BuildRootSignature()
{
	CD3DX12_DESCRIPTOR_RANGE TexTable;
	TexTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	
	// Root parameter can be a table, root descriptor or root constants.
	CD3DX12_ROOT_PARAMETER SlotRootParameter[4];

	// Perfomance TIP: Order from most frequent to least frequent.
	SlotRootParameter[0].InitAsDescriptorTable(0, &TexTable, D3D12_SHADER_VISIBILITY_PIXEL);
	SlotRootParameter[1].InitAsConstantBufferView(0);
	SlotRootParameter[1].InitAsConstantBufferView(1);
	SlotRootParameter[1].InitAsConstantBufferView(2);
	
	auto StaticSamplers = GetStaticSamplers();

	// A root signature is an array of root parameters.
	CD3DX12_ROOT_SIGNATURE_DESC RootSigDesc(4, SlotRootParameter,
		(UINT)StaticSamplers.size(), StaticSamplers.data(), 
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
	);
	
	// create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
	ComPtr<ID3DBlob> SerializedRootSig = nullptr;
	ComPtr<ID3DBlob> ErrorBlob = nullptr;
	HRESULT HR = D3D12SerializeRootSignature(&RootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
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
			IID_PPV_ARGS(RootSignature.GetAddressOf())));
}

void FCrateApp::BuildShadersAndInputLayout()
{
	Shaders["standardVS"] = d3dUtil::CompileShader(L"Shaders\\Default.hlsl", nullptr, "VS", "vs_5_0");
	Shaders["opaquePS"] = d3dUtil::CompileShader(L"Shaders\\Default.hlsl", nullptr, "PS", "ps_5_0");

	InputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};
}

void FCrateApp::BuildShapeGeometry()
{
	FGeometryGenerator GeoGen; 
	FGeometryGenerator::FMeshData Box = GeoGen.CreateBox(1.0f, 1.0f, 1.0f, 3);
	
	FSubmeshGeometry BoxSubMesh;
	BoxSubMesh.IndexCount = (UINT)Box.Indices32.size();
	BoxSubMesh.StartIndexLocation = 0;
	BoxSubMesh.BaseVertexLocation = 0;
	
	std::vector<FVertex> Vertices(Box.Vertices.size());
	for (size_t i = 0; i < Box.Vertices.size(); ++i)
	{
		Vertices[i].Pos = Box.Vertices[i].Position;
		Vertices[i].Normal = Box.Vertices[i].Position;
		Vertices[i].TexC = Box.Vertices[i].TexC;
	}
	
	std::vector<std::uint16_t> Indices = Box.GetIndices16();

	const UINT VBByteSize = (UINT)Vertices.size() * sizeof(FVertex);
	const UINT IBByteSize = (UINT)Indices.size() + sizeof(std::uint16_t);
}

void FCrateApp::BuildPSO()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC OpaquePSODesc;

	//
	// PSO for opaque objects.
	//
	ZeroMemory(&OpaquePSODesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	OpaquePSODesc.InputLayout = { InputLayout.data(), (UINT)InputLayout.size() };
	OpaquePSODesc.pRootSignature = RootSignature.Get();
	OpaquePSODesc.VS =
	{
		reinterpret_cast<BYTE*>(Shaders["standardVS"]->GetBufferPointer()),
		Shaders["standardVS"]->GetBufferSize() };
	OpaquePSODesc.PS =
	{
		reinterpret_cast<BYTE*>(Shaders["opaquePS"]->GetBufferPointer()),
		Shaders["opaquePS"]->GetBufferSize() };
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

void FCrateApp::DrawRenderItems(ID3D12GraphicsCommandList* CommandList, const std::vector<FRenderItem*>& RenderItems)
{
	UINT ObjCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(FObjectConstants));
	UINT MatCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(FMaterialConstants));

	auto ObjectCB = CurrentFrameResource->ObjectCB->Resource();
	auto MatCB = CurrentFrameResource->MaterialCB->Resource();

	// For each render item...
	for (size_t i = 0; i < RenderItems.size(); ++i)
	{
		FRenderItem* Item = RenderItems[i];

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



std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> FCrateApp::GetStaticSamplers()
{
	// Applications usually only need a handful of samplers.  So just define them all up front
	// and keep them available as part of the root signature. 
	
	const CD3DX12_STATIC_SAMPLER_DESC PointWrap(
		0,
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP
	); // addressW
	
	const CD3DX12_STATIC_SAMPLER_DESC PointClamp(
		1, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP
	); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC LinearWrap(
		2, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP
	); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC LinearClamp(
		3, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP
	); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC AnisotropicWrap(
		4, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressW
		0.0f,                             // mipLODBias
		8
	);       

	const CD3DX12_STATIC_SAMPLER_DESC AnisotropicClamp(
		5, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressW
		0.0f,                              // mipLODBias
		8
	);
	return { PointWrap, PointClamp, LinearWrap, LinearClamp, AnisotropicWrap, AnisotropicClamp };
}

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	
	try
	{
		FCrateApp App(hInst);
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
