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
#include "GPUWaves.h"

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

struct FTreeSpriteVertex
{
	XMFLOAT3 Pos;
	XMFLOAT2 Size;
};

enum class URenderLayer : int
{
	Opaque = 0,
	Transparent,
	AlphaTested,
	GpuWaves,
	Count
};

class FWavesCS : public FD3DApp
{
public:
	FWavesCS(HINSTANCE hInstance);
	~FWavesCS();

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
	void UpdateMaterialCBs(const GameTimer& gt);
	void AnimateMaterials(const GameTimer& gt);

	void LoadTextures();
	void BuildRootSignature();
	void BuildWavesRootSignature();
	void BuildDescriptorHeaps();
	void BuildShadersAndInputLayout();
	void BuildLandGeometry();
	void BuildWavesGeometry();
	void BuildBoxGeometry();
	void BuildPSOs();
	void BuildFrameResources();
	void BuildMaterials();
	void BuildRenderItems();
	void DrawRenderItems(ID3D12GraphicsCommandList *cmdList, const std::vector<FRenderItem *> &ritems);

	std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> FWavesCS::GetStaticSamplers();
	float GetHillsHeight(float x, float z) const;
	XMFLOAT3 GetHillsNormal(float x, float z) const;

private:
	std::vector<std::unique_ptr<FFrameResource>> FrameResources;
	FFrameResource *CurrentFrameResource = nullptr;
	int CurrentFrameResourceIndex = 0;

	UINT CbvSrvDescriptorSize = 0;

	ComPtr<ID3D12RootSignature> RootSignature = nullptr;
	ComPtr<ID3D12RootSignature> WavesRootSignature = nullptr;

	ComPtr<ID3D12DescriptorHeap> SrvDescriptorHeap = nullptr;

	std::unordered_map<std::string, std::unique_ptr<FMeshGeometry>> Geometries;
	std::unordered_map<std::string, std::unique_ptr<FMaterial>> Materials;
	std::unordered_map<std::string, std::unique_ptr<FTexture>> Textures;

	std::unordered_map<std::string, ComPtr<ID3DBlob>> Shaders;
	std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> PSOs;

	std::vector<D3D12_INPUT_ELEMENT_DESC> InputLayout;
	std::vector<D3D12_INPUT_ELEMENT_DESC> TreeSpriteInputLayout;

	// List of all the render items.
	std::vector<std::unique_ptr<FRenderItem>> AllRenderItems;

	// Render items divided by PSO.
	std::vector<FRenderItem *> RenderLayers[(int)URenderLayer::Count]; 
	std::unique_ptr<FGPUWaves> Waves;

	FPassConstants MainPassCB;
	UINT PassCBVOffset = 0;

	bool bIsWireFrame = false;

	XMFLOAT3 EyePos = {0.0f, 0.0f, 0.0f};
	XMFLOAT4X4 View = MathHelper::Identity4x4();
	XMFLOAT4X4 Proj = MathHelper::Identity4x4();

	float Theta = 1.5f * XM_PI;
	float Phi = XM_PIDIV2 - 0.1f;
	float Radius = 100.0f;

	POINT LastMousePos;
};

FWavesCS::FWavesCS(HINSTANCE hInstance)
	: FD3DApp(hInstance)
{
}

FWavesCS::~FWavesCS()
{
}

float FWavesCS::GetHillsHeight(float X, float Z) const
{
	return 0.3f * (Z * sinf(0.1f * X) + X * cosf(0.1f * Z));
}

XMFLOAT3 FWavesCS::GetHillsNormal(float X, float Z) const
{
	XMFLOAT3 N(
		-0.03f * Z * cosf(0.1f * X) - 0.3f * cosf(0.1f * Z),
		1.0f,
		-0.3f * sinf(0.1f * X) + 0.03f * X * sinf(0.1f * Z));

	XMVECTOR UnitNormal = XMVector3Normalize(XMLoadFloat3(&N));
	XMStoreFloat3(&N, UnitNormal);

	return N;
}

bool FWavesCS::Initialize()
{
	if (!FD3DApp::Initialize())
	{
		return false;
	}

	// Reset the command list to prep for initialization commands.
	ThrowIfFailed(CommandList->Reset(DirectCmdListAlloctor.Get(), nullptr));

	CbvSrvDescriptorSize = D3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	
	Waves = std::make_unique<FGPUWaves>(D3DDevice.Get(), CommandList.Get(), 256, 256, 0.25f, 0.03f, 2.0f, 0.2f);

	LoadTextures();
	BuildRootSignature();
	BuildWavesRootSignature();
	BuildDescriptorHeaps();
	BuildShadersAndInputLayout();
	BuildLandGeometry();
	BuildWavesGeometry();
	BuildBoxGeometry();
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

void FWavesCS::OnResize()
{
	FD3DApp::OnResize();

	// The window resized, so update the aspect ratio and recompute the projection matrix.
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	DirectX::XMStoreFloat4x4(&Proj, P);
}

void FWavesCS::Update(const GameTimer &gt)
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
	UpdateWaves(gt);
}

void FWavesCS::Draw(const GameTimer &gt)
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

	CommandList->SetPipelineState(PSOs["alphaTested"].Get());
	DrawRenderItems(CommandList.Get(), RenderLayers[(int)URenderLayer::AlphaTested]);

	CommandList->SetPipelineState(PSOs["transparent"].Get());
	DrawRenderItems(CommandList.Get(), RenderLayers[(int)URenderLayer::Transparent]);

	CommandList->SetPipelineState(PSOs["wavesRender"].Get());
	DrawRenderItems(CommandList.Get(), RenderLayers[(int)URenderLayer::GpuWaves]);


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

void FWavesCS::OnMouseDown(WPARAM btnState, int x, int y)
{
	LastMousePos.x = x;
	LastMousePos.y = y;

	SetCapture(MainWnd);
}

void FWavesCS::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void FWavesCS::OnMouseMove(WPARAM btnState, int x, int y)
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

void FWavesCS::OnKeyboardInput(const GameTimer &gt)
{
	if (GetAsyncKeyState('1') & 0x8000)
		bIsWireFrame = true;
	else
		bIsWireFrame = false;
}

void FWavesCS::UpdateCamera(const GameTimer &gt)
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

void FWavesCS::UpdateObjectCBs(const GameTimer &gt)
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

void FWavesCS::UpdateMaterialCBs(const GameTimer& gt)
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

void FWavesCS::AnimateMaterials(const GameTimer& gt)
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

void FWavesCS::UpdateMainPassCB(const GameTimer &gt)
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

void FWavesCS::UpdateWaves(const GameTimer& gt)
{
	// Every quarter second, generate a random wave.
	static float t_base = 0.0f;
	if ((Timer.TotalTime() - t_base) >= 0.25f)
	{
		t_base += 0.25f;

		int i = MathHelper::Rand(4, Waves->RowCount() - 5);
		int j = MathHelper::Rand(4, Waves->ColumnCount() - 5);

		float r = MathHelper::RandF(1.0f, 2.0f);

		Waves->Disturb(CommandList.Get(), WavesRootSignature.Get(), PSOs["wavesDisturb"].Get(), i, j, r);
	}

	// Update the wave simulation.
	Waves->Update(gt, CommandList.Get(), WavesRootSignature.Get(), PSOs["wavesUpdate"].Get());
}

void FWavesCS::LoadTextures()
{
	auto GrassTex = std::make_unique<FTexture>();
	GrassTex->Name = "grassTex";
	GrassTex->Filename = d3dUtil::GetPath(L"Textures/grass.dds");
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(D3DDevice.Get(),
		CommandList.Get(), GrassTex->Filename.c_str(),
		GrassTex->Resource, GrassTex->UploadHeap));

	auto WaterTex = std::make_unique<FTexture>();
	WaterTex->Name = "waterTex";
	WaterTex->Filename = d3dUtil::GetPath(L"Textures/water1.dds");
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(D3DDevice.Get(),
		CommandList.Get(), WaterTex->Filename.c_str(),
		WaterTex->Resource, WaterTex->UploadHeap));

	auto FenceTex = std::make_unique<FTexture>();
	FenceTex->Name = "fenceTex";
	FenceTex->Filename = d3dUtil::GetPath(L"Textures/WireFence.dds");
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(D3DDevice.Get(),
		CommandList.Get(), FenceTex->Filename.c_str(),
		FenceTex->Resource, FenceTex->UploadHeap));

	auto TreeArrayTex = std::make_unique<FTexture>();
	TreeArrayTex->Name = "treeArrayTex";
	TreeArrayTex->Filename = d3dUtil::GetPath(L"Textures/treeArray2.dds");
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(D3DDevice.Get(),
		CommandList.Get(), TreeArrayTex->Filename.c_str(),
		TreeArrayTex->Resource, TreeArrayTex->UploadHeap));

	Textures[GrassTex->Name] = std::move(GrassTex);
	Textures[WaterTex->Name] = std::move(WaterTex);
	Textures[FenceTex->Name] = std::move(FenceTex);
	Textures[TreeArrayTex->Name] = std::move(TreeArrayTex);
}


void FWavesCS::BuildRootSignature()
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

void FWavesCS::BuildWavesRootSignature()
{
	CD3DX12_DESCRIPTOR_RANGE UavTable0;
	UavTable0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);

	CD3DX12_DESCRIPTOR_RANGE UavTable1;
	UavTable1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 1);

	CD3DX12_DESCRIPTOR_RANGE UavTable2;
	UavTable2.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 2);
	
	// Root parameter can be a table, root descriptor or root constants.
	CD3DX12_ROOT_PARAMETER SlotRootParameter[4];
	
	SlotRootParameter[0].InitAsConstants(6, 0);
	SlotRootParameter[1].InitAsDescriptorTable(1, &UavTable0);
	SlotRootParameter[2].InitAsDescriptorTable(1, &UavTable1);
	SlotRootParameter[3].InitAsDescriptorTable(1, &UavTable2);
	
	// A root signature is an array of root parameters.
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(4, SlotRootParameter,
		0, nullptr,
		D3D12_ROOT_SIGNATURE_FLAG_NONE);
	
	// create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer	
	ComPtr<ID3DBlob> serializedRootSig = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}
	ThrowIfFailed(hr);

	ThrowIfFailed(D3DDevice->CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(WavesRootSignature.GetAddressOf())));
}

void FWavesCS::BuildDescriptorHeaps()
{
	UINT SrvCount = 3;

	//
	// Create the SRV heap.
	//
	D3D12_DESCRIPTOR_HEAP_DESC SrvHeapDesc = {};
	SrvHeapDesc.NumDescriptors = SrvCount + Waves->DescriptorCount();
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

	Waves->BuildDescriptors(
		CD3DX12_CPU_DESCRIPTOR_HANDLE(SrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), SrvCount, CbvSrvDescriptorSize),
		CD3DX12_GPU_DESCRIPTOR_HANDLE(SrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart(), SrvCount, CbvSrvDescriptorSize),
		CbvSrvDescriptorSize
	);
}

void FWavesCS::BuildShadersAndInputLayout()
{
	const D3D_SHADER_MACRO defines[] =
	{
		"FOG", "1",
		NULL, NULL
	};

	const D3D_SHADER_MACRO alphaTestDefines[] =
	{
		"FOG", "1",
		"ALPHA_TEST", "1",
		NULL, NULL
	};

	const D3D_SHADER_MACRO waveDefines[] =
	{
		"DISPLACEMENT_MAP", "1",
		NULL, NULL
	};

	Shaders["standardVS"] = d3dUtil::CompileShader(L"Shaders\\13_WavesCS\\Default.hlsl", nullptr, "VS", "vs_5_0");
	Shaders["wavesVS"] = d3dUtil::CompileShader(L"Shaders\\13_WavesCS\\Default.hlsl", waveDefines, "VS", "vs_5_0");
	Shaders["opaquePS"] = d3dUtil::CompileShader(L"Shaders\\13_WavesCS\\Default.hlsl", defines, "PS", "ps_5_0");
	Shaders["alphaTestedPS"] = d3dUtil::CompileShader(L"Shaders\\13_WavesCS\\Default.hlsl", alphaTestDefines, "PS", "ps_5_0");
	Shaders["wavesUpdateCS"] = d3dUtil::CompileShader(L"Shaders\\13_WavesCS\\WaveSim.hlsl", nullptr, "UpdateWavesCS", "cs_5_0");
	Shaders["wavesDisturbCS"] = d3dUtil::CompileShader(L"Shaders\\13_WavesCS\\WaveSim.hlsl", nullptr, "DisturbWavesCS", "cs_5_0");


	InputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};
}

void FWavesCS::BuildLandGeometry()
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
		auto& Pos = Grid.Vertices[i].Position;
		Vertices[i].Pos = Pos;
		Vertices[i].Pos.y = GetHillsHeight(Pos.x, Pos.z);
		Vertices[i].Normal = GetHillsNormal(Pos.x, Pos.z);
		Vertices[i].TexC = Grid.Vertices[i].TexC;
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

void FWavesCS::BuildWavesGeometry()
{
	FGeometryGenerator geoGen;
	FGeometryGenerator::FMeshData grid = geoGen.CreateGrid(160.0f, 160.0f, Waves->RowCount(), Waves->ColumnCount());

	std::vector<FVertex> vertices(grid.Vertices.size());
	for (size_t i = 0; i < grid.Vertices.size(); ++i)
	{
		vertices[i].Pos = grid.Vertices[i].Position;
		vertices[i].Normal = grid.Vertices[i].Normal;
		vertices[i].TexC = grid.Vertices[i].TexC;
	}

	std::vector<std::uint32_t> indices = grid.Indices32;

	UINT vbByteSize = Waves->GetVertexCount() * sizeof(FVertex);
	UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint32_t);

	auto geo = std::make_unique<FMeshGeometry>();
	geo->Name = "waterGeo";

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
	geo->IndexFormat = DXGI_FORMAT_R32_UINT;
	geo->IndexBufferByteSize = ibByteSize;

	FSubmeshGeometry submesh;
	submesh.IndexCount = (UINT)indices.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	geo->DrawArgs["grid"] = submesh;

	Geometries["waterGeo"] = std::move(geo);
}

void FWavesCS::BuildBoxGeometry()
{
	FGeometryGenerator GeoGen;
	FGeometryGenerator::FMeshData Box = GeoGen.CreateBox(8.0f, 8.0f, 8.0f, 3);

	FSubmeshGeometry BoxSubmesh;
	BoxSubmesh.IndexCount = (UINT)Box.Indices32.size();
	BoxSubmesh.StartIndexLocation = 0;
	BoxSubmesh.BaseVertexLocation = 0;

	std::vector<FVertex> Vertices(Box.Vertices.size());

	for (int i = 0; i < Box.Vertices.size(); ++i)
	{
		Vertices[i].Pos = Box.Vertices[i].Position;
		Vertices[i].Normal = Box.Vertices[i].Normal;
		Vertices[i].TexC = Box.Vertices[i].TexC;
	}

	std::vector<std::uint16_t>  Indices = Box.GetIndices16();

	const UINT VBByteSize = (UINT)Vertices.size() * sizeof(FVertex);
	const UINT IBByteSize = (UINT)Indices.size() * sizeof(std::uint16_t);

	auto Geo = std::make_unique<FMeshGeometry>();
	Geo->Name = "boxGeo";
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

	Geo->DrawArgs["box"] = BoxSubmesh;

	Geometries[Geo->Name] = std::move(Geo);
}

void FWavesCS::BuildMaterials()
{
	auto Grass = std::make_unique<FMaterial>();
	Grass->Name = "grass";
	Grass->MatCBIndex = 0;
	Grass->DiffuseSrvHeapIndex = 0;
	Grass->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	Grass->FresnelR0 = XMFLOAT3(0.01f, 0.01f, 0.01f);
	Grass->Roughness = 0.125f;

	// This is not a good water material definition, but we do not have all the rendering
	// tools we need (transparency, environment reflection), so we fake it for now.
	auto Water = std::make_unique<FMaterial>();
	Water->Name = "water";
	Water->MatCBIndex = 1;
	Water->DiffuseSrvHeapIndex = 1;
	Water->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);
	Water->FresnelR0 = XMFLOAT3(0.2f, 0.2f, 0.2f);
	Water->Roughness = 0.0f;

	auto Wirefence = std::make_unique<FMaterial>();
	Wirefence->Name = "wirefence";
	Wirefence->MatCBIndex = 2;
	Wirefence->DiffuseSrvHeapIndex = 2;
	Wirefence->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	Wirefence->FresnelR0 = XMFLOAT3(0.1f, 0.1f, 0.1f);
	Wirefence->Roughness = 0.25f;


	auto TreeSprites = std::make_unique<FMaterial>();
	TreeSprites->Name = "treeSprites";
	TreeSprites->MatCBIndex = 3;
	TreeSprites->DiffuseSrvHeapIndex = 3;
	TreeSprites->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	TreeSprites->FresnelR0 = XMFLOAT3(0.01f, 0.01f, 0.01f);
	TreeSprites->Roughness = 0.125f;

	Materials["grass"] = std::move(Grass);
	Materials["water"] = std::move(Water);
	Materials["wirefence"] = std::move(Wirefence);
	Materials["treeSprites"] = std::move(TreeSprites);
}

void FWavesCS::BuildRenderItems()
{
	auto WavesRitem = std::make_unique<FRenderItem>();
	WavesRitem->World = MathHelper::Identity4x4();
	XMStoreFloat4x4(&WavesRitem->TexTransform, XMMatrixScaling(5.0f, 5.0f, 1.0f));
	WavesRitem->ObjCBIndex = 0;
	WavesRitem->Mat = Materials["water"].get();
	WavesRitem->Geo = Geometries["waterGeo"].get();
	WavesRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	WavesRitem->IndexCount = WavesRitem->Geo->DrawArgs["grid"].IndexCount;
	WavesRitem->StartIndexLocation = WavesRitem->Geo->DrawArgs["grid"].StartIndexLocation;
	WavesRitem->BaseVertexLocation = WavesRitem->Geo->DrawArgs["grid"].BaseVertexLocation;

	RenderLayers[(int)URenderLayer::GpuWaves].push_back(WavesRitem.get());

	auto GridRitem = std::make_unique<FRenderItem>();
	GridRitem->World = MathHelper::Identity4x4();
	XMStoreFloat4x4(&GridRitem->TexTransform, XMMatrixScaling(5.0f, 5.0f, 1.0f));
	GridRitem->ObjCBIndex = 1;
	GridRitem->Mat = Materials["grass"].get();
	GridRitem->Geo = Geometries["landGeo"].get();
	GridRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	GridRitem->IndexCount = GridRitem->Geo->DrawArgs["grid"].IndexCount;
	GridRitem->StartIndexLocation = GridRitem->Geo->DrawArgs["grid"].StartIndexLocation;
	GridRitem->BaseVertexLocation = GridRitem->Geo->DrawArgs["grid"].BaseVertexLocation;

	RenderLayers[(int)URenderLayer::Opaque].push_back(GridRitem.get());

	auto BoxRitem = std::make_unique<FRenderItem>();
	XMStoreFloat4x4(&BoxRitem->World, XMMatrixTranslation(3.0f, 2.0f, -9.0f));
	BoxRitem->ObjCBIndex = 2;
	BoxRitem->Mat = Materials["wirefence"].get();
	BoxRitem->Geo = Geometries["boxGeo"].get();
	BoxRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	BoxRitem->IndexCount = BoxRitem->Geo->DrawArgs["box"].IndexCount;
	BoxRitem->StartIndexLocation = BoxRitem->Geo->DrawArgs["box"].StartIndexLocation;
	BoxRitem->BaseVertexLocation = BoxRitem->Geo->DrawArgs["box"].BaseVertexLocation;

	RenderLayers[(int)URenderLayer::AlphaTested].push_back(BoxRitem.get());

	AllRenderItems.push_back(std::move(WavesRitem));
	AllRenderItems.push_back(std::move(GridRitem));
	AllRenderItems.push_back(std::move(BoxRitem));
}

void FWavesCS::DrawRenderItems(ID3D12GraphicsCommandList *CommandList, const std::vector<FRenderItem *> &RenderItems)
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

void FWavesCS::BuildFrameResources()
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

void FWavesCS::BuildPSOs()
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
	// PSO for alpha tested objects
	//	
	D3D12_GRAPHICS_PIPELINE_STATE_DESC AlphaTestedPsoDesc = OpaquePSODesc;
	AlphaTestedPsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(Shaders["alphaTestedPS"]->GetBufferPointer()),
		Shaders["alphaTestedPS"]->GetBufferSize()
	};
	AlphaTestedPsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	ThrowIfFailed(
		D3DDevice->CreateGraphicsPipelineState(&AlphaTestedPsoDesc, IID_PPV_ARGS(&PSOs["alphaTested"]))
	);
	
	//
	// PSO for drawing waves
	//
	D3D12_GRAPHICS_PIPELINE_STATE_DESC WavesRenderPSO = TransparentPsoDesc;
	WavesRenderPSO.VS =
	{
		reinterpret_cast<BYTE*>(Shaders["wavesVS"]->GetBufferPointer()),
		Shaders["wavesVS"]->GetBufferSize()
	};
	ThrowIfFailed(D3DDevice->CreateGraphicsPipelineState(&WavesRenderPSO, IID_PPV_ARGS(&PSOs["wavesRender"])));


	//
	// PSO for disturbing waves
	//
	D3D12_COMPUTE_PIPELINE_STATE_DESC wavesDisturbPSO = {};
	wavesDisturbPSO.pRootSignature = WavesRootSignature.Get();
	wavesDisturbPSO.CS =
	{
		reinterpret_cast<BYTE*>(Shaders["wavesDisturbCS"]->GetBufferPointer()),
		Shaders["wavesDisturbCS"]->GetBufferSize()
	};
	wavesDisturbPSO.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	ThrowIfFailed(D3DDevice->CreateComputePipelineState(&wavesDisturbPSO, IID_PPV_ARGS(&PSOs["wavesDisturb"])));

	//
	// PSO for updating waves
	//
	D3D12_COMPUTE_PIPELINE_STATE_DESC wavesUpdatePSO = {};
	wavesUpdatePSO.pRootSignature = WavesRootSignature.Get();
	wavesUpdatePSO.CS =
	{
		reinterpret_cast<BYTE*>(Shaders["wavesUpdateCS"]->GetBufferPointer()),
		Shaders["wavesUpdateCS"]->GetBufferSize()
	};
	wavesUpdatePSO.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	ThrowIfFailed(D3DDevice->CreateComputePipelineState(&wavesUpdatePSO, IID_PPV_ARGS(&PSOs["wavesUpdate"])));
}

std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> FWavesCS::GetStaticSamplers()
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
		FWavesCS App(hInst);
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
