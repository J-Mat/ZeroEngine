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
// using namespace DirectX::PackedVector;

const int gNumFrameResources = 3;

#define ITEM_SHAPE "Shape"
#define ITEM_BOX "Box"
#define ITEM_GRID "Grid"
#define ITEM_SPHERE "Sphere"
#define ITEM_CYLINDER "Cylinder"

enum class URenderLayer : int
{
	Opaque = 0,
	Mirrors,
	Reflected,
	Transparent,
	Shadow,
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

enum class URenderLayer : int
{
	Opaque = 0,
	Transparent,
	AlphaTested,
	Count
};

class FStencil : public FD3DApp
{
public:
	FStencil(HINSTANCE hInstance);
	~FStencil();

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
	void UpdateMaterialCBs(const GameTimer& gt);
	void UpdateReflectedPassCB(const GameTimer& gt);
	void AnimateMaterials(const GameTimer& gt);

	void LoadTextures();
	void BuildRootSignature();
	void BuildDescriptorHeaps();
	void BuildShadersAndInputLayout();
	void BuildSkullGeometry();
	void BuildRoomGeometry();
	void BuildPSOs();
	void BuildFrameResources();
	void BuildMaterials();
	void BuildRenderItems();
	void DrawRenderItems(ID3D12GraphicsCommandList *cmdList, const std::vector<FRenderItem *> &ritems);

	std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> FStencil::GetStaticSamplers();
	float GetHillsHeight(float x, float z) const;
	XMFLOAT3 GetHillsNormal(float x, float z) const;

private:
	std::vector<std::unique_ptr<FFrameResource>> FrameResources;
	FFrameResource *CurrentFrameResource = nullptr;
	int CurrentFrameResourceIndex = 0;

	UINT CbvSrvDescriptorSize = 0;

	ComPtr<ID3D12RootSignature> RootSignature = nullptr;

	ComPtr<ID3D12DescriptorHeap> SrvDescriptorHeap = nullptr;

	std::unordered_map<std::string, std::unique_ptr<FMeshGeometry>> Geometries;
	std::unordered_map<std::string, std::unique_ptr<FMaterial>> Materials;
	std::unordered_map<std::string, std::unique_ptr<FTexture>> Textures;

	std::unordered_map<std::string, ComPtr<ID3DBlob>> Shaders;
	std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> PSOs;


	// Cache render items of interest.
	FRenderItem* SkullRenderitem = nullptr;
	FRenderItem* ReflectedSkullRenderitem = nullptr;
	FRenderItem* ShadowedSkullRenderitem = nullptr;

	std::vector<D3D12_INPUT_ELEMENT_DESC> InputLayout;



	// List of all the render items.
	std::vector<std::unique_ptr<FRenderItem>> AllRenderItems;

	// Render items divided by PSO.
	std::vector<FRenderItem *> RenderLayers[(int)URenderLayer::Count]; 

	FPassConstants MainPassCB;
	FPassConstants ReflectedPassCB;
	UINT PassCBVOffset = 0;

	XMFLOAT3 SkullTranslation = { 0.0f, 1.0f, -5.0f };

	XMFLOAT3 EyePos = {0.0f, 0.0f, 0.0f};
	XMFLOAT4X4 View = MathHelper::Identity4x4();
	XMFLOAT4X4 Proj = MathHelper::Identity4x4();

	float Theta = 1.5f * XM_PI;
	float Phi = XM_PIDIV2 - 0.1f;
	float Radius = 50.0f;

	POINT LastMousePos;
};

FStencil::FStencil(HINSTANCE hInstance)
	: FD3DApp(hInstance)
{
}

FStencil::~FStencil()
{
}

float FStencil::GetHillsHeight(float X, float Z) const
{
	return 0.3f * (Z * sinf(0.1f * X) + X * cosf(0.1f * Z));
}

XMFLOAT3 FStencil::GetHillsNormal(float X, float Z) const
{
	XMFLOAT3 N(
		-0.03f * Z * cosf(0.1f * X) - 0.3f * cosf(0.1f * Z),
		1.0f,
		-0.3f * sinf(0.1f * X) + 0.03f * X * sinf(0.1f * Z));

	XMVECTOR UnitNormal = XMVector3Normalize(XMLoadFloat3(&N));
	XMStoreFloat3(&N, UnitNormal);

	return N;
}

bool FStencil::Initialize()
{
	if (!FD3DApp::Initialize())
	{
		return false;
	}

	// Reset the command list to prep for initialization commands.
	ThrowIfFailed(CommandList->Reset(DirectCmdListAlloctor.Get(), nullptr));

	CbvSrvDescriptorSize = D3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	

	LoadTextures();
	BuildRootSignature();
	BuildDescriptorHeaps();
	BuildShadersAndInputLayout();
	BuildRoomGeometry();
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

void FStencil::OnResize()
{
	FD3DApp::OnResize();

	// The window resized, so update the aspect ratio and recompute the projection matrix.
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	DirectX::XMStoreFloat4x4(&Proj, P);
}

void FStencil::Update(const GameTimer &gt)
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

	AnimateMaterials(gt);
	UpdateObjectCBs(gt);
	UpdateMaterialCBs(gt);
	UpdateMainPassCB(gt);
	UpdateReflectedPassCB(gt);
}

void FStencil::Draw(const GameTimer &gt)
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

	// Bind per-pass constant buffer.  We only need to do this once per-pass.
	auto PassCB = CurrentFrameResource->PassCB->Resource();
	CommandList->SetGraphicsRootConstantBufferView(2, PassCB->GetGPUVirtualAddress());
	DrawRenderItems(CommandList.Get(), RenderLayers[(int)URenderLayer::Opaque]);

	CommandList->OMSetStencilRef(1);
	CommandList->SetPipelineState(PSOs["markStencilMirrors"].Get());
	DrawRenderItems(CommandList.Get(), RenderLayers[(int)URenderLayer::Mirrors]);
	
	// Draw the reflection into the mirror only (only for pixels where the stencil buffer is 1).
	// Note that we must supply a different per-pass constant buffer--one with the lights reflected.
	CommandList->SetGraphicsRootConstantBufferView(2, PassCB);
	CommandList->SetPipelineState(PSOs["drawStencilReflections"].Get());
	DrawRenderItems(CommandList.Get(), RenderLayers[(int)URenderLayer::Reflected]);

	CommandList->SetPipelineState(PSOs["transparent"].Get());
	DrawRenderItems(CommandList.Get(), RenderLayers[(int)URenderLayer::Transparent]);


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

void FStencil::OnMouseDown(WPARAM btnState, int x, int y)
{
	LastMousePos.x = x;
	LastMousePos.y = y;

	SetCapture(MainWnd);
}

void FStencil::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void FStencil::OnMouseMove(WPARAM btnState, int x, int y)
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

void FStencil::OnKeyboardInput(const GameTimer &gt)
{
	//
	// Allow user to move skull.
	//

	const float dt = gt.DeltaTime();
	if (GetAsyncKeyState('A') & 0x8000)
		SkullTranslation.x -= 1.0f * dt;

	if (GetAsyncKeyState('D') & 0x8000)
		SkullTranslation.x += 1.0f * dt;

	if (GetAsyncKeyState('W') & 0x8000)
		SkullTranslation.y += 1.0f * dt;

	if (GetAsyncKeyState('S') & 0x8000)
		SkullTranslation.y -= 1.0f * dt;
	
	// Don't let user move below ground plane.
	SkullTranslation.y = MathHelper::Max(SkullTranslation.y, 0.0f);
	// Update the new world matrix.
	XMMATRIX SkullRotate = XMMatrixRotationY(0.5f * MathHelper::Pi);
	XMMATRIX SkullScale = XMMatrixScaling(0.45f, 0.45f, 0.45f);
	XMMATRIX SkullOffset = XMMatrixTranslation(SkullTranslation.x, SkullTranslation.y, SkullTranslation.z);
	XMMATRIX SkullWorld = SkullRotate * SkullScale * SkullOffset;
	XMStoreFloat4x4(&SkullRenderitem->World, SkullWorld);
	
	// Update reflection world matrix.
	XMVECTOR MirrorPlane = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f); // xy plane
	XMMATRIX R = XMMatrixReflect(MirrorPlane);
	XMStoreFloat4x4(&ReflectedSkullRenderitem->World, SkullWorld * R);

	// Update shadow world matrix.
	XMVECTOR ShadowPlane = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); // xz plane
	XMVECTOR ToMainLight = -XMLoadFloat3(&MainPassCB.Lights[0].Direction);
	XMMATRIX S = XMMatrixShadow(ShadowPlane, ToMainLight);
	XMMATRIX ShadowOffsetY = XMMatrixTranslation(0.0f, 0.001f, 0.0f);
	XMStoreFloat4x4(&ShadowedSkullRenderitem->World,  SkullWorld * S * ShadowOffsetY);


	SkullRenderitem->NumFrameDirty = gNumFrameResources;
	ReflectedSkullRenderitem->NumFrameDirty = gNumFrameResources;
	ShadowedSkullRenderitem->NumFrameDirty = gNumFrameResources;
}

void FStencil::UpdateCamera(const GameTimer &gt)
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

void FStencil::UpdateObjectCBs(const GameTimer &gt)
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

void FStencil::UpdateMaterialCBs(const GameTimer& gt)
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

void FStencil::UpdateReflectedPassCB(const GameTimer& gt)
{
	ReflectedPassCB = MainPassCB;

	XMVECTOR MirrorPlane = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f); // xy plane
	XMMATRIX R = XMMatrixReflect(MirrorPlane);

	// Reflect the lighting.
	for (int i = 0; i < 3; ++i)
	{
		XMVECTOR LightDir = XMLoadFloat3(&MainPassCB.Lights[i].Direction);
		XMVECTOR ReflectedLightDir = XMVector3TransformNormal(LightDir, R);
		XMStoreFloat3(&ReflectedPassCB.Lights[i].Direction, ReflectedLightDir);
	}
	
	// Reflected pass stored in index 1
	auto CurrPassCB = CurrentFrameResource->PassCB.get();
	CurrPassCB->CopyData(1, ReflectedPassCB);
}

void FStencil::AnimateMaterials(const GameTimer& gt)
{
	// Scroll the water material texture coordinates.
	auto waterMat = Materials["water"].get();

	float& tu = waterMat->MatTransform(3, 0);
	float& tv = waterMat->MatTransform(3, 1);

	tu += 0.1f * gt.DeltaTime();
	tv += 0.02f * gt.DeltaTime();

	if (tu >= 1.0f)
		tu -= 1.0f;

	if (tv >= 1.0f)
		tv -= 1.0f;

	waterMat->MatTransform(3, 0) = tu;
	waterMat->MatTransform(3, 1) = tv;

	// Material has changed, so need to update cbuffer.
	waterMat->NumFramesDirty = gNumFrameResources;
}

void FStencil::UpdateMainPassCB(const GameTimer &gt)
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
	MainPassCB.Lights[0].Strength = { 0.9f, 0.9f, 0.8f };
	MainPassCB.Lights[1].Direction = { -0.57735f, -0.57735f, 0.57735f };
	MainPassCB.Lights[1].Strength = { 0.3f, 0.3f, 0.3f };
	MainPassCB.Lights[2].Direction = { 0.0f, -0.707f, -0.707f };
	MainPassCB.Lights[2].Strength = { 0.15f, 0.15f, 0.15f };

	auto CurPassCB = CurrentFrameResource->PassCB.get();
	CurPassCB->CopyData(0, MainPassCB);
}

void FStencil::LoadTextures()
{
	auto bricksTex = std::make_unique<FTexture>();
	bricksTex->Name = "bricksTex";
	bricksTex->Filename = d3dUtil::GetPath(L"Textures/bricks3.dds");
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(D3DDevice.Get(),
		CommandList.Get(), bricksTex->Filename.c_str(),
		bricksTex->Resource, bricksTex->UploadHeap));

	auto checkboardTex = std::make_unique<FTexture>();
	checkboardTex->Name = "checkboardTex";
	checkboardTex->Filename = d3dUtil::GetPath(L"Textures/checkboard.dds");
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(D3DDevice.Get(),
		CommandList.Get(), checkboardTex->Filename.c_str(),
		checkboardTex->Resource, checkboardTex->UploadHeap));

	auto iceTex = std::make_unique<FTexture>();
	iceTex->Name = "iceTex";
	iceTex->Filename = d3dUtil::GetPath(L"Textures/ice.dds");
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(D3DDevice.Get(),
		CommandList.Get(), iceTex->Filename.c_str(),
		iceTex->Resource, iceTex->UploadHeap));

	auto white1x1Tex = std::make_unique<FTexture>();
	white1x1Tex->Name = "white1x1Tex";
	white1x1Tex->Filename = d3dUtil::GetPath(L"Textures/white1x1.dds");
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(D3DDevice.Get(),
		CommandList.Get(), white1x1Tex->Filename.c_str(),
		white1x1Tex->Resource, white1x1Tex->UploadHeap));

	Textures[bricksTex->Name] = std::move(bricksTex);
	Textures[checkboardTex->Name] = std::move(checkboardTex);
	Textures[iceTex->Name] = std::move(iceTex);
	Textures[white1x1Tex->Name] = std::move(white1x1Tex);
}


void FStencil::BuildRootSignature()
{
	CD3DX12_DESCRIPTOR_RANGE TexTable;
	TexTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

	// Root parameter can be a table, root descriptor or root constants.
	CD3DX12_ROOT_PARAMETER SlotRootParameter[4];

	// Perfomance TIP: Order from most frequent to least frequent.
	SlotRootParameter[0].InitAsDescriptorTable(1, &TexTable, D3D12_SHADER_VISIBILITY_PIXEL);
	SlotRootParameter[1].InitAsConstantBufferView(0);
	SlotRootParameter[2].InitAsConstantBufferView(1);
	SlotRootParameter[3].InitAsConstantBufferView(2);

	auto staticSamplers = GetStaticSamplers();

	// A root signature is an array of root parameters.
	CD3DX12_ROOT_SIGNATURE_DESC RootSigDesc(4, SlotRootParameter,
		(UINT)staticSamplers.size(), staticSamplers.data(),
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

void FStencil::BuildDescriptorHeaps()
{
	//
	// Create the SRV heap.
	//
	D3D12_DESCRIPTOR_HEAP_DESC SrvHeapDesc = {};
	SrvHeapDesc.NumDescriptors = 3;
	SrvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	SrvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	ThrowIfFailed(D3DDevice->CreateDescriptorHeap(&SrvHeapDesc, IID_PPV_ARGS(&SrvDescriptorHeap)));

	//
	// Fill out the heap with actual descriptors.
	//
	CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(SrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	auto GrassTex = Textures["grassTex"]->Resource;
	auto WaterTex = Textures["waterTex"]->Resource;
	auto FenceTex = Textures["fenceTex"]->Resource;

	D3D12_SHADER_RESOURCE_VIEW_DESC SrvDesc = {};
	SrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	SrvDesc.Format = GrassTex->GetDesc().Format;
	SrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	SrvDesc.Texture2D.MostDetailedMip = 0;
	SrvDesc.Texture2D.MipLevels = -1;
	D3DDevice->CreateShaderResourceView(GrassTex.Get(), &SrvDesc, hDescriptor);

	// next descriptor
	hDescriptor.Offset(1, CbvSrvDescriptorSize);

	SrvDesc.Format = WaterTex->GetDesc().Format;
	D3DDevice->CreateShaderResourceView(WaterTex.Get(), &SrvDesc, hDescriptor);

	// next descriptor
	hDescriptor.Offset(1, CbvSrvDescriptorSize);

	SrvDesc.Format = FenceTex->GetDesc().Format;
	D3DDevice->CreateShaderResourceView(FenceTex.Get(), &SrvDesc, hDescriptor);
}

void FStencil::BuildShadersAndInputLayout()
{
	const D3D_SHADER_MACRO Defines[] =
	{
		"FOG", "1",
		NULL, NULL
	};

	const D3D_SHADER_MACRO AlphaTestDefines[] =
	{
		"FOG", "1",
		"ALPHA_TEST", "1",
		NULL, NULL
	};


	Shaders["standardVS"] = d3dUtil::CompileShader(L"Shaders\\10_Blend\\Default.hlsl", nullptr, "VS", "vs_5_0");
	Shaders["opaquePS"] = d3dUtil::CompileShader(L"Shaders\\10_Blend\\Default.hlsl", Defines, "PS", "ps_5_0");
	Shaders["alphaTestedPS"] = d3dUtil::CompileShader(L"Shaders\\10_Blend\\Default.hlsl", AlphaTestDefines, "PS", "ps_5_0");

	InputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};
}

void FStencil::BuildRoomGeometry()
{
	// Create and specify geometry.  For this sample we draw a floor
	// and a wall with a mirror on it.  We put the floor, wall, and
	// mirror geometry in one vertex buffer.
	//
	//   |--------------|
	//   |              |
	//   |----|----|----|
	//   |Wall|Mirr|Wall|
	//   |    | or |    |
	//   /--------------/
	//  /   Floor      /
	// /--------------/
	std::array<FVertex, 20> Vertices =
	{
		// Floor: Observe we tile texture coordinates.
		FVertex(-3.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 4.0f), // 0 
		FVertex(-3.5f, 0.0f,   0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f),
		FVertex(7.5f, 0.0f,   0.0f, 0.0f, 1.0f, 0.0f, 4.0f, 0.0f),
		FVertex(7.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 4.0f, 4.0f),

		// Wall: Observe we tile texture coordinates, and that we
		// leave a gap in the middle for the mirror.
		FVertex(-3.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 2.0f), // 4
		FVertex(-3.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f),
		FVertex(-2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.5f, 0.0f),
		FVertex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.5f, 2.0f),

		FVertex(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 2.0f), // 8 
		FVertex(2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f),
		FVertex(7.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 2.0f, 0.0f),
		FVertex(7.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 2.0f, 2.0f),

		FVertex(-3.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f), // 12
		FVertex(-3.5f, 6.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f),
		FVertex(7.5f, 6.0f, 0.0f, 0.0f, 0.0f, -1.0f, 6.0f, 0.0f),
		FVertex(7.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 6.0f, 1.0f),

		// Mirror
		FVertex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f), // 16
		FVertex(-2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f),
		FVertex(2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f),
		FVertex(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f)
	};
	

	std::array<std::int16_t, 30> Indices =
	{
		// Floor
		0, 1, 2,
		0, 2, 3,

		// Walls
		4, 5, 6,
		4, 6, 7,

		8, 9, 10,
		8, 10, 11,

		12, 13, 14,
		12, 14, 15,

		// Mirror
		16, 17, 18,
		16, 18, 19
	};

	FSubmeshGeometry FloorSubmesh;
	FloorSubmesh.IndexCount = 6;
	FloorSubmesh.StartIndexLocation = 0;
	FloorSubmesh.BaseVertexLocation = 0;

	FSubmeshGeometry WallSubmesh;
	WallSubmesh.IndexCount = 18;
	WallSubmesh.StartIndexLocation = 6;
	WallSubmesh.BaseVertexLocation = 0;

	FSubmeshGeometry MirrorSubmesh;
	MirrorSubmesh.IndexCount = 6;
	MirrorSubmesh.StartIndexLocation = 24;
	MirrorSubmesh.BaseVertexLocation = 0;

	const UINT VbByteSize = (UINT)Vertices.size() * sizeof(FVertex);
	const UINT IbByteSize = (UINT)Indices.size() * sizeof(std::uint16_t);

	auto Geo = std::make_unique<FMeshGeometry>();
	Geo->Name = "roomGeo";

	ThrowIfFailed(D3DCreateBlob(VbByteSize, &Geo->VertexBufferCPU));
	CopyMemory(Geo->VertexBufferCPU->GetBufferPointer(), Vertices.data(), VbByteSize);

	ThrowIfFailed(D3DCreateBlob(IbByteSize, &Geo->IndexBufferCPU));
	CopyMemory(Geo->IndexBufferCPU->GetBufferPointer(), Indices.data(), IbByteSize);

	Geo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(D3DDevice.Get(),
		CommandList.Get(), Vertices.data(), VbByteSize, Geo->VertexBufferUploader);

	Geo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(D3DDevice.Get(),
		CommandList.Get(), Indices.data(), IbByteSize, Geo->IndexBufferUploader);

	Geo->VertexByteStride = sizeof(FVertex);
	Geo->VertexBufferByteSize = VbByteSize;
	Geo->IndexFormat = DXGI_FORMAT_R16_UINT;
	Geo->IndexBufferByteSize = IbByteSize;

	Geo->DrawArgs["floor"] = FloorSubmesh;
	Geo->DrawArgs["wall"] = WallSubmesh;
	Geo->DrawArgs["mirror"] = MirrorSubmesh;

	Geometries[Geo->Name] = std::move(Geo);
}

void FStencil::BuildSkullGeometry()
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


void FStencil::BuildMaterials()
{
	auto Bricks = std::make_unique<FMaterial>();
	Bricks->Name = "bricks";
	Bricks->MatCBIndex = 0;
	Bricks->DiffuseSrvHeapIndex = 0;
	Bricks->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	Bricks->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
	Bricks->Roughness = 0.25f;

	auto Checkertile = std::make_unique<FMaterial>();
	Checkertile->Name = "checkertile";
	Checkertile->MatCBIndex = 1;
	Checkertile->DiffuseSrvHeapIndex = 1;
	Checkertile->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	Checkertile->FresnelR0 = XMFLOAT3(0.07f, 0.07f, 0.07f);
	Checkertile->Roughness = 0.3f;

	auto Icemirror = std::make_unique<FMaterial>();
	Icemirror->Name = "icemirror";
	Icemirror->MatCBIndex = 2;
	Icemirror->DiffuseSrvHeapIndex = 2;
	Icemirror->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.3f);
	Icemirror->FresnelR0 = XMFLOAT3(0.1f, 0.1f, 0.1f);
	Icemirror->Roughness = 0.5f;

	auto SkullMat = std::make_unique<FMaterial>();
	SkullMat->Name = "skullMat";
	SkullMat->MatCBIndex = 3;
	SkullMat->DiffuseSrvHeapIndex = 3;
	SkullMat->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SkullMat->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
	SkullMat->Roughness = 0.3f;

	auto ShadowMat = std::make_unique<FMaterial>();
	ShadowMat->Name = "shadowMat";
	ShadowMat->MatCBIndex = 4;
	ShadowMat->DiffuseSrvHeapIndex = 3;
	ShadowMat->DiffuseAlbedo = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.5f);
	ShadowMat->FresnelR0 = XMFLOAT3(0.001f, 0.001f, 0.001f);
	ShadowMat->Roughness = 0.0f;

	Materials["bricks"] = std::move(Bricks);
	Materials["checkertile"] = std::move(Checkertile);
	Materials["icemirror"] = std::move(Icemirror);
	Materials["skullMat"] = std::move(SkullMat);
	Materials["shadowMat"] = std::move(ShadowMat);
}

void FStencil::BuildRenderItems()
{
	auto FloorRitem = std::make_unique<FRenderItem>();
	FloorRitem->World = MathHelper::Identity4x4();
	FloorRitem->TexTransform = MathHelper::Identity4x4();
	FloorRitem->ObjCBIndex = 0;
	FloorRitem->Mat = Materials["checkertile"].get();
	FloorRitem->Geo = Geometries["roomGeo"].get();
	FloorRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	FloorRitem->IndexCount = FloorRitem->Geo->DrawArgs["floor"].IndexCount;
	FloorRitem->StartIndexLocation = FloorRitem->Geo->DrawArgs["floor"].StartIndexLocation;
	FloorRitem->BaseVertexLocation = FloorRitem->Geo->DrawArgs["floor"].BaseVertexLocation;
	RenderLayers[(int)URenderLayer::Opaque].push_back(FloorRitem.get());

	auto WallsRitem = std::make_unique<FRenderItem>();
	WallsRitem->World = MathHelper::Identity4x4();
	WallsRitem->TexTransform = MathHelper::Identity4x4();
	WallsRitem->ObjCBIndex = 1;
	WallsRitem->Mat = Materials["bricks"].get();
	WallsRitem->Geo = Geometries["roomGeo"].get();
	WallsRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	WallsRitem->IndexCount = WallsRitem->Geo->DrawArgs["wall"].IndexCount;
	WallsRitem->StartIndexLocation = WallsRitem->Geo->DrawArgs["wall"].StartIndexLocation;
	WallsRitem->BaseVertexLocation = WallsRitem->Geo->DrawArgs["wall"].BaseVertexLocation;
	RenderLayers[(int)URenderLayer::Opaque].push_back(WallsRitem.get());

	auto SkullRitem = std::make_unique<FRenderItem>();
	SkullRitem->World = MathHelper::Identity4x4();
	SkullRitem->TexTransform = MathHelper::Identity4x4();
	SkullRitem->ObjCBIndex = 2;
	SkullRitem->Mat = Materials["skullMat"].get();
	SkullRitem->Geo = Geometries["skullGeo"].get();
	SkullRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	SkullRitem->IndexCount = SkullRitem->Geo->DrawArgs["skull"].IndexCount;
	SkullRitem->StartIndexLocation = SkullRitem->Geo->DrawArgs["skull"].StartIndexLocation;
	SkullRitem->BaseVertexLocation = SkullRitem->Geo->DrawArgs["skull"].BaseVertexLocation;
	SkullRenderitem = SkullRitem.get();
	RenderLayers[(int)URenderLayer::Opaque].push_back(SkullRitem.get());

	// Reflected skull will have different world matrix, so it needs to be its own render item.
	auto ReflectedSkullRitem = std::make_unique<FRenderItem>();
	*ReflectedSkullRitem = *SkullRitem;
	ReflectedSkullRitem->ObjCBIndex = 3;
	ReflectedSkullRenderitem = ReflectedSkullRitem.get();
	RenderLayers[(int)URenderLayer::Reflected].push_back(ReflectedSkullRitem.get());

	// Shadowed skull will have different world matrix, so it needs to be its own render item.
	auto shadowedSkullRitem = std::make_unique<FRenderItem>();
	*shadowedSkullRitem = *SkullRitem;
	shadowedSkullRitem->ObjCBIndex = 4;
	shadowedSkullRitem->Mat = Materials["shadowMat"].get();
	ShadowedSkullRenderitem = shadowedSkullRitem.get();
	RenderLayers[(int)URenderLayer::Shadow].push_back(shadowedSkullRitem.get());

	auto MirrorRitem = std::make_unique<FRenderItem>();
	MirrorRitem->World = MathHelper::Identity4x4();
	MirrorRitem->TexTransform = MathHelper::Identity4x4();
	MirrorRitem->ObjCBIndex = 5;
	MirrorRitem->Mat = Materials["icemirror"].get();
	MirrorRitem->Geo = Geometries["roomGeo"].get();
	MirrorRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	MirrorRitem->IndexCount = MirrorRitem->Geo->DrawArgs["mirror"].IndexCount;
	MirrorRitem->StartIndexLocation = MirrorRitem->Geo->DrawArgs["mirror"].StartIndexLocation;
	MirrorRitem->BaseVertexLocation = MirrorRitem->Geo->DrawArgs["mirror"].BaseVertexLocation;
	RenderLayers[(int)URenderLayer::Mirrors].push_back(MirrorRitem.get());
	RenderLayers[(int)URenderLayer::Transparent].push_back(MirrorRitem.get());

	AllRenderItems.push_back(std::move(FloorRitem));
	AllRenderItems.push_back(std::move(WallsRitem));
	AllRenderItems.push_back(std::move(SkullRitem));
	AllRenderItems.push_back(std::move(ReflectedSkullRitem));
	AllRenderItems.push_back(std::move(shadowedSkullRitem));
	AllRenderItems.push_back(std::move(MirrorRitem));
}

void FStencil::DrawRenderItems(ID3D12GraphicsCommandList *CommandList, const std::vector<FRenderItem *> &RenderItems)
{
	UINT ObjCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(FObjectConstants));
	UINT MatCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(FMaterialConstants));

	auto ObjectCB = CurrentFrameResource->ObjectCB->Resource();
	auto MatCB = CurrentFrameResource->MaterialCB->Resource();

	for (int i = 0; i < RenderItems.size(); ++i)
	{
		FRenderItem* Item = RenderItems[i];

		CommandList->IASetVertexBuffers(0, 1, &Item->Geo->VertexBufferView());
		CommandList->IASetIndexBuffer(&Item->Geo->IndexBufferView());
		CommandList->IASetPrimitiveTopology(Item->PrimitiveType);

		CD3DX12_GPU_DESCRIPTOR_HANDLE Tex(SrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
		Tex.Offset(Item->Mat->DiffuseSrvHeapIndex, CbvSrvUavDescriptorSize);

		D3D12_GPU_VIRTUAL_ADDRESS ObjCBAddress = ObjectCB->GetGPUVirtualAddress() + Item->ObjCBIndex * ObjCBByteSize;
		D3D12_GPU_VIRTUAL_ADDRESS MatCBAddress = MatCB->GetGPUVirtualAddress() + Item->Mat->MatCBIndex * MatCBByteSize;

		CommandList->SetGraphicsRootDescriptorTable(0, Tex);
		CommandList->SetGraphicsRootConstantBufferView(1, ObjCBAddress);
		CommandList->SetGraphicsRootConstantBufferView(3, MatCBAddress);

		CommandList->DrawIndexedInstanced(Item->IndexCount, 1, Item->StartIndexLocation, Item->BaseVertexLocation, 0);
	}
}

void FStencil::BuildFrameResources()
{
	for (int i = 0; i < gNumFrameResources; ++i)
	{
		FrameResources.push_back(
			std::make_unique<FFrameResource>(
				D3DDevice.Get(),
				1,
				(UINT)AllRenderItems.size(),
				(UINT)Materials.size()
				));
	}
}

void FStencil::BuildPSOs()
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
		Shaders["standardVS"]->GetBufferSize()
	};
	OpaquePSODesc.PS =
	{
		reinterpret_cast<BYTE*>(Shaders["opaquePS"]->GetBufferPointer()),
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
		D3DDevice->CreateGraphicsPipelineState(&OpaquePSODesc, IID_PPV_ARGS(&PSOs["opaque"]))
	);

	//
	// PSO for transparent objects
	//
	D3D12_GRAPHICS_PIPELINE_STATE_DESC TransparentPsoDesc = OpaquePSODesc;

	D3D12_RENDER_TARGET_BLEND_DESC TransparencyBlendDesc;
	TransparencyBlendDesc.BlendEnable = true;
	TransparencyBlendDesc.LogicOpEnable = false;
	TransparencyBlendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	TransparencyBlendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	TransparencyBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
	TransparencyBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	TransparencyBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
	TransparencyBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	TransparencyBlendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
	TransparencyBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	TransparentPsoDesc.BlendState.RenderTarget[0] = TransparencyBlendDesc;
	ThrowIfFailed(
		D3DDevice->CreateGraphicsPipelineState(&TransparentPsoDesc, IID_PPV_ARGS(&PSOs["transparent"]))
	);
	//
	// PSO for marking stencil mirrors.
	//	
	CD3DX12_BLEND_DESC MirrorBlendState(D3D12_DEFAULT);
	MirrorBlendState.RenderTarget[0].RenderTargetWriteMask = 0;
	
	D3D12_DEPTH_STENCIL_DESC MirrorDSS;
	MirrorDSS.DepthEnable = true;
	MirrorDSS.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	MirrorDSS.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	MirrorDSS.StencilEnable = true;
	MirrorDSS.StencilReadMask = 0xff;
	MirrorDSS.StencilWriteMask = 0xff;
	
	MirrorDSS.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	MirrorDSS.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	MirrorDSS.FrontFace.StencilPassOp = D3D12_STENCIL_OP_REPLACE;
	MirrorDSS.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

	// We are not rendering backfacing polygons, so these settings do not matter.
	MirrorDSS.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	MirrorDSS.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	MirrorDSS.BackFace.StencilPassOp = D3D12_STENCIL_OP_REPLACE;
	MirrorDSS.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC MarkMirrorsPsoDesc = OpaquePSODesc;
	MarkMirrorsPsoDesc.BlendState = MirrorBlendState;
	MarkMirrorsPsoDesc.DepthStencilState = MirrorDSS;
	ThrowIfFailed(
		D3DDevice->CreateGraphicsPipelineState(&MarkMirrorsPsoDesc, IID_PPV_ARGS(&PSOs["markStencilMirrors"]))
	);
	

	//
	// PSO for stencil reflections.
	//
	D3D12_DEPTH_STENCIL_DESC ReflectionsDSS;
	ReflectionsDSS.DepthEnable = true;
	ReflectionsDSS.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	ReflectionsDSS.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	ReflectionsDSS.StencilEnable = true;
	ReflectionsDSS.StencilReadMask = 0xff;
	ReflectionsDSS.StencilWriteMask = 0xff;
	
	ReflectionsDSS.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	ReflectionsDSS.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	ReflectionsDSS.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	ReflectionsDSS.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;
	

	// We are not rendering backfacing polygons, so these settings do not matter.
	ReflectionsDSS.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	ReflectionsDSS.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	ReflectionsDSS.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	ReflectionsDSS.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;
	
	D3D12_GRAPHICS_PIPELINE_STATE_DESC DrawRefectionsPsoDesc = OpaquePSODesc;
	DrawRefectionsPsoDesc.DepthStencilState = ReflectionsDSS;
	DrawRefectionsPsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	DrawRefectionsPsoDesc.RasterizerState.FrontCounterClockwise = true;
	ThrowIfFailed(
		D3DDevice->CreateGraphicsPipelineState(&DrawRefectionsPsoDesc, IID_PPV_ARGS(&PSOs["drawStencilReflections"]))
	);
	
	//
	// PSO for shadow objects
	//
	// We are going to draw shadows with transparency, so base it off the transparency description.
	D3D12_DEPTH_STENCIL_DESC ShadowDSS;
	ShadowDSS.DepthEnable = true;
	ShadowDSS.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	ShadowDSS.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	ShadowDSS.StencilEnable = true;
	ShadowDSS.StencilReadMask = 0xff;
	ShadowDSS.StencilWriteMask = 0xff;
	
	ShadowDSS.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	ShadowDSS.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	ShadowDSS.FrontFace.StencilPassOp = D3D12_STENCIL_OP_INCR;
	ShadowDSS.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;
	
	ShadowDSS.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	ShadowDSS.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	ShadowDSS.BackFace.StencilPassOp = D3D12_STENCIL_OP_INCR;
	ShadowDSS.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC ShadowPsoDesc = OpaquePSODesc;
	ShadowPsoDesc.DepthStencilState = ShadowDSS;
	ThrowIfFailed(
		D3DDevice->CreateGraphicsPipelineState(&ShadowPsoDesc, IID_PPV_ARGS(&PSOs["shadow"]))
	);
	
}


std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> FStencil::GetStaticSamplers()
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
		FStencil App(hInst);
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
