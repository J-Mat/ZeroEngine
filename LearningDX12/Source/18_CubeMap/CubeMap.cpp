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
#include "Camera.h"

using namespace DirectX;
using namespace Microsoft::WRL;
// using namespace DirectX::PackedVector;
const int gNumFrameResources = 3;

enum class URenderLayer : int
{
	Opaque = 0,
	Sky,
	Count
};

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

class CameraApp : public FD3DApp
{
public:
	CameraApp(HINSTANCE hInstance);
	~CameraApp();

	virtual bool Initialize() override;

private:
	virtual void OnResize() override;
	virtual void Update(const GameTimer &gt) override;
	virtual void Draw(const GameTimer &gt) override;

	virtual void OnMouseDown(WPARAM btnState, int x, int y) override;
	virtual void OnMouseUp(WPARAM btnState, int x, int y) override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y) override;

	void OnKeyboardInput(const GameTimer &gt);
	void UpdateObjectCBs(const GameTimer &gt);
	void UpdateMaterialBuffer(const GameTimer &gt);
	void UpdateMainPassCB(const GameTimer &gt);

	void LoadTextures();
	void BuildRootSignature();
	void BuildMaterials();
	void BuildDescriptorHeaps();
	void BuildShadersAndInputLayout();
	void BuildShapeGeometry();
	void BuildSkullGeometry();
	void BuildPSOs();
	void BuildFrameResources();
	void BuildRenderItems();
	void DrawRenderItems(ID3D12GraphicsCommandList *cmdList, const std::vector<FRenderItem *> &ritems);
	std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();

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
	std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> PSOs;
	std::vector<D3D12_INPUT_ELEMENT_DESC> InputLayout;


	std::vector<FRenderItem*> RenderLayers[(int)URenderLayer::Count];
	// List of all the render items.
	std::vector<std::unique_ptr<FRenderItem>> AllRenderItems;

	FPassConstants MainPassCB;
	UINT PassCBVOffset = 0;

	bool bIsWireFrame = false;

	float Theta = 1.5f * XM_PI;
	float Phi = XM_PIDIV4;
	float Radius = 15.0f;

	UINT SkyTexHeapIndex = 0;

	FCamera Camera;

	POINT LastMousePos;
};

CameraApp::CameraApp(HINSTANCE hInstance)
	: FD3DApp(hInstance)
{
}

CameraApp::~CameraApp()
{
}

bool CameraApp::Initialize()
{
	if (!FD3DApp::Initialize())
	{
		return false;
	}

	// Reset the command list to prep for initialization commands.
	ThrowIfFailed(
		CommandList->Reset(DirectCmdListAlloctor.Get(), nullptr));
	
	CbvSrvUavDescriptorSize = D3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	Camera.SetPosition(0.0f, 2.0f, -15.0f);
	
	LoadTextures();
	BuildRootSignature();
	BuildDescriptorHeaps();
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

void CameraApp::OnResize()
{
	FD3DApp::OnResize();

	Camera.SetLens(0.25f * MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
}

void CameraApp::Update(const GameTimer &gt)
{
	OnKeyboardInput(gt);

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
	UpdateMaterialBuffer(gt);
	UpdateMainPassCB(gt);
}

void CameraApp::Draw(const GameTimer &gt)
{
	auto CmdListAlloc = CurrentFrameResource->CommandListAllocator;

	// Reuse the memory associated with command recording.
	// We can only reset when the associated command lists have finished execution on th
	ThrowIfFailed(CmdListAlloc->Reset());

	// A command list can be reset after it has been added to the command queue via ExecuteCommandList.
	// Reusing the command list reuses memory.

	ThrowIfFailed(CommandList->Reset(CmdListAlloc.Get(), PSOs["opaque"].Get()));

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

	ID3D12DescriptorHeap* DescriptorHeaps[] = { SrvDescriptorHeap.Get() };
	CommandList->SetDescriptorHeaps(_countof(DescriptorHeaps), DescriptorHeaps);

	CommandList->SetGraphicsRootSignature(RootSignature.Get());

	// Bind all the materials used in this scene.  For structured buffers, we can bypass the heap and 
	// set as a root descriptor.
	auto PassCB = CurrentFrameResource->PassCB->Resource();
	CommandList->SetGraphicsRootConstantBufferView(1, PassCB->GetGPUVirtualAddress());

	auto MatBuffer = CurrentFrameResource->MaterialBuffer->Resource();
	CommandList->SetGraphicsRootShaderResourceView(2, MatBuffer->GetGPUVirtualAddress());


	// Bind the sky cube map.  For our demos, we just use one "world" cube map representing the environment
	// from far away, so all objects will use the same cube map and we only need to set it once per-frame.  
	// If we wanted to use "local" cube maps, we would have to change them per-object, or dynamically
	// index into an array of cube maps.

	CD3DX12_GPU_DESCRIPTOR_HANDLE SkyTexDescriptor(SrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	SkyTexDescriptor.Offset(SkyTexHeapIndex, CbvSrvUavDescriptorSize);
	CommandList->SetGraphicsRootDescriptorTable(3, SkyTexDescriptor);
	
	// Bind all the textures used in this scene.  Observe
	// that we only have to specify the first descriptor in the table.  
	// The root signature knows how many descriptors are expected in the table.
	CommandList->SetGraphicsRootDescriptorTable(4, SrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	DrawRenderItems(CommandList.Get(), RenderLayers[(int)URenderLayer::Opaque]);
	
	CommandList->SetPipelineState(PSOs["sky"].Get());
	DrawRenderItems(CommandList.Get(), RenderLayers[(int)URenderLayer::Sky]);

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

void CameraApp::OnMouseDown(WPARAM btnState, int x, int y)
{
	LastMousePos.x = x;
	LastMousePos.y = y;

	SetCapture(MainWnd);
}

void CameraApp::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void CameraApp::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f * static_cast<float>(x - LastMousePos.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(y - LastMousePos.y));

		Camera.Pitch(dy);
		Camera.RotateY(dx);
	}

	LastMousePos.x = x;
	LastMousePos.y = y;
}

void CameraApp::OnKeyboardInput(const GameTimer &gt)
{
	const float dt = gt.DeltaTime();

	if (GetAsyncKeyState('W') & 0x8000)
		Camera.Walk(10.0f * dt);

	if (GetAsyncKeyState('S') & 0x8000)
		Camera.Walk(-10.0f * dt);

	if (GetAsyncKeyState('A') & 0x8000)
		Camera.Strafe(-10.0f * dt);

	if (GetAsyncKeyState('D') & 0x8000)
		Camera.Strafe(10.0f * dt);

	Camera.UpdateViewMatrix();
}

void CameraApp::UpdateObjectCBs(const GameTimer &gt)
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
			ObjConstants.MaterialIndex = Item->Mat->MatCBIndex;

			CurrentObjectCB->CopyData(Item->ObjCBIndex, ObjConstants);

			Item->NumFrameDirty--;
		}
	}
}

void CameraApp::UpdateMaterialBuffer(const GameTimer &gt)
{
	auto CurrentMaterialCB = CurrentFrameResource->MaterialBuffer.get();
	for (auto &E : Materials)
	{
		// Only update the cbuffer data if the constants have changed.  If the cbuffer
		// data changes, it needs to be updated for each FrameResource.
		FMaterial *Mat = E.second.get();
		if (Mat->NumFramesDirty > 0)
		{
			XMMATRIX TransForm = XMLoadFloat4x4(&Mat->MatTransform);

			FMaterialData MaterialData;
			MaterialData.DiffuseAlbedo = Mat->DiffuseAlbedo;
			MaterialData.FresnelR0 = Mat->FresnelR0;
			MaterialData.Roughness = Mat->Roughness;
			XMStoreFloat4x4(&MaterialData.MatTransform, XMMatrixTranspose(TransForm));
			MaterialData.DiffuseMapIndex = Mat->DiffuseSrvHeapIndex;

			CurrentMaterialCB->CopyData(Mat->MatCBIndex, MaterialData);

			--Mat->NumFramesDirty;
		}
	}
}

void CameraApp::UpdateMainPassCB(const GameTimer &gt)
{
	XMMATRIX ViewMatrix = Camera.GetView();
	XMMATRIX ProjMatrix = Camera.GetProj();

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
	MainPassCB.EyePosW = Camera.GetPosition3f();
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

void CameraApp::LoadTextures()
{
	std::vector<std::string> TexNames =
	{
		"bricksDiffuseMap",
		"tileDiffuseMap",
		"defaultDiffuseMap",
		"skyCubeMap"
	};

	std::vector<std::wstring> texFilenames =
	{
		L"Textures/bricks2.dds",
		L"Textures/tile.dds",
		L"Textures/white1x1.dds",
		L"Textures/grasscube1024.dds"
	};
	for (int i = 0; i < (int)TexNames.size(); ++i)
	{
		auto TexMap = std::make_unique<FTexture>();
		TexMap->Name = TexNames[i];
		TexMap->Filename = d3dUtil::GetPath(texFilenames[i]);
		ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(D3DDevice.Get(),
			CommandList.Get(), TexMap->Filename.c_str(),
			TexMap->Resource, TexMap->UploadHeap));

		Textures[TexMap->Name] = std::move(TexMap);
	}
}

void CameraApp::BuildRootSignature()
{
	CD3DX12_DESCRIPTOR_RANGE TexTable0;
	TexTable0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0);

	CD3DX12_DESCRIPTOR_RANGE TexTable1;
	TexTable1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 5, 1, 0);
	
	// Root parameter can be a table, root descriptor or root constants.
	CD3DX12_ROOT_PARAMETER SlotRootParameter[5];

	// Perfomance TIP: Order from most frequent to least frequent.
	SlotRootParameter[0].InitAsConstantBufferView(0);
	SlotRootParameter[1].InitAsConstantBufferView(1);
	SlotRootParameter[2].InitAsShaderResourceView(0, 1);
	SlotRootParameter[3].InitAsDescriptorTable(1, &TexTable0, D3D12_SHADER_VISIBILITY_PIXEL);
	SlotRootParameter[4].InitAsDescriptorTable(1, &TexTable1, D3D12_SHADER_VISIBILITY_PIXEL);


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


void CameraApp::BuildDescriptorHeaps()
{
	//
	// Create the SRV heap.
	//
	D3D12_DESCRIPTOR_HEAP_DESC SrvHeapDesc = {};
	SrvHeapDesc.NumDescriptors = 4;
	SrvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	SrvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	ThrowIfFailed(
		D3DDevice->CreateDescriptorHeap(&SrvHeapDesc, IID_PPV_ARGS(&SrvDescriptorHeap))
	);

	//
	// Fill out the heap with actual descriptors.
	//
	CD3DX12_CPU_DESCRIPTOR_HANDLE Descriptor(SrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
	
	auto BricksTex = Textures["bricksDiffuseMap"]->Resource;
	auto TileTex = Textures["tileDiffuseMap"]->Resource;
	auto WhiteTex = Textures["defaultDiffuseMap"]->Resource;
	auto SkyTex = Textures["skyCubeMap"]->Resource;

	D3D12_SHADER_RESOURCE_VIEW_DESC SrvDesc = {};
	SrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	SrvDesc.Format = BricksTex->GetDesc().Format;
	SrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	SrvDesc.Texture2D.MostDetailedMip = 0;
	SrvDesc.Texture2D.MipLevels = BricksTex->GetDesc().MipLevels;
	SrvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	D3DDevice->CreateShaderResourceView(BricksTex.Get(), &SrvDesc, Descriptor);

	// next descriptor
	Descriptor.Offset(1, CbvSrvUavDescriptorSize);

	SrvDesc.Format = TileTex->GetDesc().Format;
	SrvDesc.Texture2D.MipLevels = TileTex->GetDesc().MipLevels;
	D3DDevice->CreateShaderResourceView(TileTex.Get(), &SrvDesc, Descriptor);

	// next descriptor
	Descriptor.Offset(1, CbvSrvUavDescriptorSize);

	SrvDesc.Format = WhiteTex->GetDesc().Format;
	SrvDesc.Texture2D.MipLevels = WhiteTex->GetDesc().MipLevels;
	D3DDevice->CreateShaderResourceView(WhiteTex.Get(), &SrvDesc, Descriptor);

	// next descriptor
	Descriptor.Offset(1, CbvSrvUavDescriptorSize);

	SrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
	SrvDesc.TextureCube.MostDetailedMip = 0;
	SrvDesc.TextureCube.MipLevels = SkyTex->GetDesc().MipLevels;
	SrvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
	SrvDesc.Format = SkyTex->GetDesc().Format;
	D3DDevice->CreateShaderResourceView(SkyTex.Get(), &SrvDesc, Descriptor);

	SkyTexHeapIndex = 3;
}

void CameraApp::BuildMaterials()
{
	auto bricks0 = std::make_unique<FMaterial>();
	bricks0->Name = "bricks0";
	bricks0->MatCBIndex = 0;
	bricks0->DiffuseSrvHeapIndex = 0;
	bricks0->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	bricks0->FresnelR0 = XMFLOAT3(0.1f, 0.1f, 0.1f);
	bricks0->Roughness = 0.3f;

	auto tile0 = std::make_unique<FMaterial>();
	tile0->Name = "tile0";
	tile0->MatCBIndex = 1;
	tile0->DiffuseSrvHeapIndex = 1;
	tile0->DiffuseAlbedo = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	tile0->FresnelR0 = XMFLOAT3(0.2f, 0.2f, 0.2f);
	tile0->Roughness = 0.1f;

	auto mirror0 = std::make_unique<FMaterial>();
	mirror0->Name = "mirror0";
	mirror0->MatCBIndex = 2;
	mirror0->DiffuseSrvHeapIndex = 2;
	mirror0->DiffuseAlbedo = XMFLOAT4(0.0f, 0.0f, 0.1f, 1.0f);
	mirror0->FresnelR0 = XMFLOAT3(0.98f, 0.97f, 0.95f);
	mirror0->Roughness = 0.1f;

	auto skullMat = std::make_unique<FMaterial>();
	skullMat->Name = "skullMat";
	skullMat->MatCBIndex = 3;
	skullMat->DiffuseSrvHeapIndex = 2;
	skullMat->DiffuseAlbedo = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	skullMat->FresnelR0 = XMFLOAT3(0.2f, 0.2f, 0.2f);
	skullMat->Roughness = 0.2f;

	auto sky = std::make_unique<FMaterial>();
	sky->Name = "sky";
	sky->MatCBIndex = 4;
	sky->DiffuseSrvHeapIndex = 3;
	sky->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	sky->FresnelR0 = XMFLOAT3(0.1f, 0.1f, 0.1f);
	sky->Roughness = 1.0f;

	Materials["bricks0"] = std::move(bricks0);
	Materials["tile0"] = std::move(tile0);
	Materials["mirror0"] = std::move(mirror0);
	Materials["skullMat"] = std::move(skullMat);
	Materials["sky"] = std::move(sky);
}

void CameraApp::BuildShadersAndInputLayout()
{
	const D3D_SHADER_MACRO alphaTestDefines[] =
	{
		"ALPHA_TEST", "1",
		NULL, NULL
	};

	Shaders["standardVS"] = d3dUtil::CompileShader(L"Shaders\\18_CubeMap\\Default.hlsl", nullptr, "VS", "vs_5_1");
	Shaders["opaquePS"] = d3dUtil::CompileShader(L"Shaders\\18_CubeMap\\Default.hlsl", nullptr, "PS", "ps_5_1");

	Shaders["skyVS"] = d3dUtil::CompileShader(L"Shaders\\18_CubeMap\\Sky.hlsl", nullptr, "VS", "vs_5_1");
	Shaders["skyPS"] = d3dUtil::CompileShader(L"Shaders\\18_CubeMap\\Sky.hlsl", nullptr, "PS", "ps_5_1");

	InputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};
}

void CameraApp::BuildShapeGeometry()
{
	FGeometryGenerator geoGen;
	FGeometryGenerator::FMeshData box = geoGen.CreateBox(1.0f, 1.0f, 1.0f, 3);
	FGeometryGenerator::FMeshData grid = geoGen.CreateGrid(20.0f, 30.0f, 60, 40);
	FGeometryGenerator::FMeshData sphere = geoGen.CreateSphere(0.5f, 20, 20);
	FGeometryGenerator::FMeshData cylinder = geoGen.CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20);

	//
	// We are concatenating all the geometry into one big vertex/index buffer.  So
	// define the regions in the buffer each submesh covers.
	//

	// Cache the vertex offsets to each object in the concatenated vertex buffer.
	UINT boxVertexOffset = 0;
	UINT gridVertexOffset = (UINT)box.Vertices.size();
	UINT sphereVertexOffset = gridVertexOffset + (UINT)grid.Vertices.size();
	UINT cylinderVertexOffset = sphereVertexOffset + (UINT)sphere.Vertices.size();

	// Cache the starting index for each object in the concatenated index buffer.
	UINT boxIndexOffset = 0;
	UINT gridIndexOffset = (UINT)box.Indices32.size();
	UINT sphereIndexOffset = gridIndexOffset + (UINT)grid.Indices32.size();
	UINT cylinderIndexOffset = sphereIndexOffset + (UINT)sphere.Indices32.size();

	FSubmeshGeometry boxSubmesh;
	boxSubmesh.IndexCount = (UINT)box.Indices32.size();
	boxSubmesh.StartIndexLocation = boxIndexOffset;
	boxSubmesh.BaseVertexLocation = boxVertexOffset;

	FSubmeshGeometry gridSubmesh;
	gridSubmesh.IndexCount = (UINT)grid.Indices32.size();
	gridSubmesh.StartIndexLocation = gridIndexOffset;
	gridSubmesh.BaseVertexLocation = gridVertexOffset;

	FSubmeshGeometry sphereSubmesh;
	sphereSubmesh.IndexCount = (UINT)sphere.Indices32.size();
	sphereSubmesh.StartIndexLocation = sphereIndexOffset;
	sphereSubmesh.BaseVertexLocation = sphereVertexOffset;

	FSubmeshGeometry cylinderSubmesh;
	cylinderSubmesh.IndexCount = (UINT)cylinder.Indices32.size();
	cylinderSubmesh.StartIndexLocation = cylinderIndexOffset;
	cylinderSubmesh.BaseVertexLocation = cylinderVertexOffset;

	//
	// Extract the vertex elements we are interested in and pack the
	// vertices of all the meshes into one vertex buffer.
	//

	auto totalVertexCount =
		box.Vertices.size() +
		grid.Vertices.size() +
		sphere.Vertices.size() +
		cylinder.Vertices.size();

	std::vector<FVertex> vertices(totalVertexCount);

	UINT k = 0;
	for (size_t i = 0; i < box.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = box.Vertices[i].Position;
		vertices[k].Normal = box.Vertices[i].Normal;
		vertices[k].TexC = box.Vertices[i].TexC;
	}

	for (size_t i = 0; i < grid.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = grid.Vertices[i].Position;
		vertices[k].Normal = grid.Vertices[i].Normal;
		vertices[k].TexC = grid.Vertices[i].TexC;
	}

	for (size_t i = 0; i < sphere.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = sphere.Vertices[i].Position;
		vertices[k].Normal = sphere.Vertices[i].Normal;
		vertices[k].TexC = sphere.Vertices[i].TexC;
	}

	for (size_t i = 0; i < cylinder.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = cylinder.Vertices[i].Position;
		vertices[k].Normal = cylinder.Vertices[i].Normal;
		vertices[k].TexC = cylinder.Vertices[i].TexC;
	}

	std::vector<std::uint16_t> indices;
	indices.insert(indices.end(), std::begin(box.GetIndices16()), std::end(box.GetIndices16()));
	indices.insert(indices.end(), std::begin(grid.GetIndices16()), std::end(grid.GetIndices16()));
	indices.insert(indices.end(), std::begin(sphere.GetIndices16()), std::end(sphere.GetIndices16()));
	indices.insert(indices.end(), std::begin(cylinder.GetIndices16()), std::end(cylinder.GetIndices16()));

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(FVertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	auto geo = std::make_unique<FMeshGeometry>();
	geo->Name = "shapeGeo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
	CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	geo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(D3DDevice.Get(),
		CommandList.Get(), vertices.data(), vbByteSize, geo->VertexBufferUploader);

	geo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(D3DDevice.Get(),
		CommandList.Get(), indices.data(), ibByteSize, geo->IndexBufferUploader);

	geo->VertexByteStride = sizeof(FVertex);
	geo->VertexBufferByteSize = vbByteSize;
	geo->IndexFormat = DXGI_FORMAT_R16_UINT;
	geo->IndexBufferByteSize = ibByteSize;

	geo->DrawArgs["box"] = boxSubmesh;
	geo->DrawArgs["grid"] = gridSubmesh;
	geo->DrawArgs["sphere"] = sphereSubmesh;
	geo->DrawArgs["cylinder"] = cylinderSubmesh;

	Geometries[geo->Name] = std::move(geo);
}

void CameraApp::BuildSkullGeometry()
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

void CameraApp::BuildRenderItems()
{
	auto skyRitem = std::make_unique<FRenderItem>();
	XMStoreFloat4x4(&skyRitem->World, XMMatrixScaling(5000.0f, 5000.0f, 5000.0f));
	skyRitem->TexTransform = MathHelper::Identity4x4();
	skyRitem->ObjCBIndex = 0;
	skyRitem->Mat = Materials["sky"].get();
	skyRitem->Geo = Geometries["shapeGeo"].get();
	skyRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	skyRitem->IndexCount = skyRitem->Geo->DrawArgs["sphere"].IndexCount;
	skyRitem->StartIndexLocation = skyRitem->Geo->DrawArgs["sphere"].StartIndexLocation;
	skyRitem->BaseVertexLocation = skyRitem->Geo->DrawArgs["sphere"].BaseVertexLocation;

	RenderLayers[(int)URenderLayer::Sky].push_back(skyRitem.get());
	AllRenderItems.push_back(std::move(skyRitem));

	auto boxRitem = std::make_unique<FRenderItem>();
	XMStoreFloat4x4(&boxRitem->World, XMMatrixScaling(2.0f, 1.0f, 2.0f) * XMMatrixTranslation(0.0f, 0.5f, 0.0f));
	XMStoreFloat4x4(&boxRitem->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	boxRitem->ObjCBIndex = 1;
	boxRitem->Mat = Materials["bricks0"].get();
	boxRitem->Geo = Geometries["shapeGeo"].get();
	boxRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	boxRitem->IndexCount = boxRitem->Geo->DrawArgs["box"].IndexCount;
	boxRitem->StartIndexLocation = boxRitem->Geo->DrawArgs["box"].StartIndexLocation;
	boxRitem->BaseVertexLocation = boxRitem->Geo->DrawArgs["box"].BaseVertexLocation;

	RenderLayers[(int)URenderLayer::Opaque].push_back(boxRitem.get());
	AllRenderItems.push_back(std::move(boxRitem));

	auto SkullRitem = std::make_unique<FRenderItem>();
	XMStoreFloat4x4(&SkullRitem->World, XMMatrixScaling(0.4f, 0.4f, 0.4f) * XMMatrixTranslation(0.0f, 1.0f, 0.0f));
	SkullRitem->TexTransform = MathHelper::Identity4x4();
	SkullRitem->ObjCBIndex = 2;
	SkullRitem->Mat = Materials["skullMat"].get();
	SkullRitem->Geo = Geometries["skullGeo"].get();
	SkullRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	SkullRitem->IndexCount = SkullRitem->Geo->DrawArgs["skull"].IndexCount;
	SkullRitem->StartIndexLocation = SkullRitem->Geo->DrawArgs["skull"].StartIndexLocation;
	SkullRitem->BaseVertexLocation = SkullRitem->Geo->DrawArgs["skull"].BaseVertexLocation;

	RenderLayers[(int)URenderLayer::Opaque].push_back(SkullRitem.get());
	AllRenderItems.push_back(std::move(SkullRitem));

	auto GridRitem = std::make_unique<FRenderItem>();
	GridRitem->World = MathHelper::Identity4x4();
	XMStoreFloat4x4(&GridRitem->TexTransform, XMMatrixScaling(8.0f, 8.0f, 1.0f));
	GridRitem->ObjCBIndex = 3;
	GridRitem->Mat = Materials["tile0"].get();
	GridRitem->Geo = Geometries["shapeGeo"].get();
	GridRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	GridRitem->IndexCount = GridRitem->Geo->DrawArgs["grid"].IndexCount;
	GridRitem->StartIndexLocation = GridRitem->Geo->DrawArgs["grid"].StartIndexLocation;
	GridRitem->BaseVertexLocation = GridRitem->Geo->DrawArgs["grid"].BaseVertexLocation;

	RenderLayers[(int)URenderLayer::Opaque].push_back(GridRitem.get());
	AllRenderItems.push_back(std::move(GridRitem));

	XMMATRIX brickTexTransform = XMMatrixScaling(1.5f, 2.0f, 1.0f);
	UINT objCBIndex = 4;
	for (int i = 0; i < 5; ++i)
	{
		auto LeftCylRitem = std::make_unique<FRenderItem>();
		auto RightCylRitem = std::make_unique<FRenderItem>();
		auto LeftSphereRitem = std::make_unique<FRenderItem>();
		auto RightSphereRitem = std::make_unique<FRenderItem>();

		XMMATRIX LeftCylWorld = XMMatrixTranslation(-5.0f, 1.5f, -10.0f + i * 5.0f);
		XMMATRIX RightCylWorld = XMMatrixTranslation(+5.0f, 1.5f, -10.0f + i * 5.0f);

		XMMATRIX LeftSphereWorld = XMMatrixTranslation(-5.0f, 3.5f, -10.0f + i * 5.0f);
		XMMATRIX RightSphereWorld = XMMatrixTranslation(+5.0f, 3.5f, -10.0f + i * 5.0f);

		XMStoreFloat4x4(&LeftCylRitem->World, RightCylWorld);
		XMStoreFloat4x4(&LeftCylRitem->TexTransform, brickTexTransform);
		LeftCylRitem->ObjCBIndex = objCBIndex++;
		LeftCylRitem->Mat = Materials["bricks0"].get();
		LeftCylRitem->Geo = Geometries["shapeGeo"].get();
		LeftCylRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		LeftCylRitem->IndexCount = LeftCylRitem->Geo->DrawArgs["cylinder"].IndexCount;
		LeftCylRitem->StartIndexLocation = LeftCylRitem->Geo->DrawArgs["cylinder"].StartIndexLocation;
		LeftCylRitem->BaseVertexLocation = LeftCylRitem->Geo->DrawArgs["cylinder"].BaseVertexLocation;

		XMStoreFloat4x4(&RightCylRitem->World, LeftCylWorld);
		XMStoreFloat4x4(&RightCylRitem->TexTransform, brickTexTransform);
		RightCylRitem->ObjCBIndex = objCBIndex++;
		RightCylRitem->Mat = Materials["bricks0"].get();
		RightCylRitem->Geo = Geometries["shapeGeo"].get();
		RightCylRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		RightCylRitem->IndexCount = RightCylRitem->Geo->DrawArgs["cylinder"].IndexCount;
		RightCylRitem->StartIndexLocation = RightCylRitem->Geo->DrawArgs["cylinder"].StartIndexLocation;
		RightCylRitem->BaseVertexLocation = RightCylRitem->Geo->DrawArgs["cylinder"].BaseVertexLocation;

		XMStoreFloat4x4(&LeftSphereRitem->World, LeftSphereWorld);
		LeftSphereRitem->TexTransform = MathHelper::Identity4x4();
		LeftSphereRitem->ObjCBIndex = objCBIndex++;
		LeftSphereRitem->Mat = Materials["mirror0"].get();
		LeftSphereRitem->Geo = Geometries["shapeGeo"].get();
		LeftSphereRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		LeftSphereRitem->IndexCount = LeftSphereRitem->Geo->DrawArgs["sphere"].IndexCount;
		LeftSphereRitem->StartIndexLocation = LeftSphereRitem->Geo->DrawArgs["sphere"].StartIndexLocation;
		LeftSphereRitem->BaseVertexLocation = LeftSphereRitem->Geo->DrawArgs["sphere"].BaseVertexLocation;

		XMStoreFloat4x4(&RightSphereRitem->World, RightSphereWorld);
		RightSphereRitem->TexTransform = MathHelper::Identity4x4();
		RightSphereRitem->ObjCBIndex = objCBIndex++;
		RightSphereRitem->Mat = Materials["mirror0"].get();
		RightSphereRitem->Geo = Geometries["shapeGeo"].get();
		RightSphereRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		RightSphereRitem->IndexCount = RightSphereRitem->Geo->DrawArgs["sphere"].IndexCount;
		RightSphereRitem->StartIndexLocation = RightSphereRitem->Geo->DrawArgs["sphere"].StartIndexLocation;
		RightSphereRitem->BaseVertexLocation = RightSphereRitem->Geo->DrawArgs["sphere"].BaseVertexLocation;

		RenderLayers[(int)URenderLayer::Opaque].push_back(LeftCylRitem.get());
		RenderLayers[(int)URenderLayer::Opaque].push_back(RightCylRitem.get());
		RenderLayers[(int)URenderLayer::Opaque].push_back(LeftSphereRitem.get());
		RenderLayers[(int)URenderLayer::Opaque].push_back(RightSphereRitem.get());

		AllRenderItems.push_back(std::move(LeftCylRitem));
		AllRenderItems.push_back(std::move(RightCylRitem));
		AllRenderItems.push_back(std::move(LeftSphereRitem));
		AllRenderItems.push_back(std::move(RightSphereRitem));
	}
}
void CameraApp::DrawRenderItems(ID3D12GraphicsCommandList *CommandList, const std::vector<FRenderItem *> &RenderItems)
{
	UINT ObjCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(FObjectConstants));

	auto ObjectCB = CurrentFrameResource->ObjectCB->Resource();

	// For each render item...
	for (size_t i = 0; i < RenderItems.size(); ++i)
	{
		FRenderItem *Item = RenderItems[i];

		CommandList->IASetVertexBuffers(0, 1, &Item->Geo->VertexBufferView());
		CommandList->IASetIndexBuffer(&Item->Geo->IndexBufferView());
		CommandList->IASetPrimitiveTopology(Item->PrimitiveType);

		D3D12_GPU_VIRTUAL_ADDRESS ObjCBAddress = ObjectCB->GetGPUVirtualAddress() + Item->ObjCBIndex * ObjCBByteSize;

		//CD3DX12_GPU_DESCRIPTOR_HANDLE Tex(SrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
		//Tex.Offset(Item->Mat->DiffuseSrvHeapIndex, CbvSrvUavDescriptorSize);
		
		CommandList->SetGraphicsRootConstantBufferView(0, ObjCBAddress);
		
		CommandList->DrawIndexedInstanced(Item->IndexCount, 1, Item->StartIndexLocation, Item->BaseVertexLocation, 0);
	}
}

void CameraApp::BuildFrameResources()
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

void CameraApp::BuildPSOs()
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
		D3DDevice->CreateGraphicsPipelineState(&OpaquePSODesc, IID_PPV_ARGS(&PSOs["opaque"])));


	//
	// PSO for sky.
	//
	D3D12_GRAPHICS_PIPELINE_STATE_DESC SkyPsoDesc = OpaquePSODesc;

	// The camera is inside the sky sphere, so just turn off culling.
	SkyPsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

	// Make sure the depth function is LESS_EQUAL and not just LESS.  
	// Otherwise, the normalized depth values at z = 1 (NDC) will 
	// fail the depth test if the depth buffer was cleared to 1.
	SkyPsoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	SkyPsoDesc.pRootSignature = RootSignature.Get();
	SkyPsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(Shaders["skyVS"]->GetBufferPointer()),
		Shaders["skyVS"]->GetBufferSize()
	};
	SkyPsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(Shaders["skyPS"]->GetBufferPointer()),
		Shaders["skyPS"]->GetBufferSize()
	};
	ThrowIfFailed(D3DDevice->CreateGraphicsPipelineState(&SkyPsoDesc, IID_PPV_ARGS(&PSOs["sky"])));
}

std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> CameraApp::GetStaticSamplers()
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
		CameraApp App(hInst);
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
