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
const int gNumDataElements = 32;

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

struct FData
{
	XMFLOAT3 v1;
	XMFLOAT2 v2;
};

enum class URenderLayer : int
{
	Opaque = 0,
	Transparent,
	AlphaTested,
	AlphaTestedTreeSprites,
	Count
};

class FVecAdd : public FD3DApp
{
public:
	FVecAdd(HINSTANCE hInstance);
	~FVecAdd();

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
	void UpdateMaterialCBs(const GameTimer& gt);

	void BuildBuffers();
	void BuildRootSignature();
	void BuildDescriptorHeaps();
	void BuildShadersAndInputLayout();
	void BuildPSOs();
	void BuildFrameResources();
	void DoComputeWork();

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

	std::vector<D3D12_INPUT_ELEMENT_DESC> InputLayout;
	std::vector<D3D12_INPUT_ELEMENT_DESC> TreeSpriteInputLayout;

	// List of all the render items.
	std::vector<std::unique_ptr<FRenderItem>> AllRenderItems;

	// Render items divided by PSO.
	std::vector<FRenderItem *> RenderLayers[(int)URenderLayer::Count]; 

	ComPtr<ID3D12Resource> InputBufferA = nullptr;
	ComPtr<ID3D12Resource> InputUploadBufferA = nullptr;
	ComPtr<ID3D12Resource> InputBufferB = nullptr;
	ComPtr<ID3D12Resource> InputUploadBufferB = nullptr;
	ComPtr<ID3D12Resource> OutputBuffer = nullptr;
	ComPtr<ID3D12Resource> ReadBackBuffer = nullptr;



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

FVecAdd::FVecAdd(HINSTANCE hInstance)
	: FD3DApp(hInstance)
{
}

FVecAdd::~FVecAdd()
{
}

bool FVecAdd::Initialize()
{
	if (!FD3DApp::Initialize())
	{
		return false;
	}

	// Reset the command list to prep for initialization commands.
	ThrowIfFailed(CommandList->Reset(DirectCmdListAlloctor.Get(), nullptr));

	CbvSrvDescriptorSize = D3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	BuildBuffers();
	BuildRootSignature();
	BuildDescriptorHeaps();
	BuildShadersAndInputLayout();
	BuildFrameResources();
	BuildPSOs();

	// Execute the initialization commands.
	ThrowIfFailed(CommandList->Close());
	ID3D12CommandList *CmdsLists[] = {CommandList.Get()};
	CommandQueue->ExecuteCommandLists(_countof(CmdsLists), CmdsLists);

	// Wait until initialization is complete.
	FlushCommandQueue();

	DoComputeWork();

	return true;
}

void FVecAdd::OnResize()
{
	FD3DApp::OnResize();

	// The window resized, so update the aspect ratio and recompute the projection matrix.
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	DirectX::XMStoreFloat4x4(&Proj, P);
}

void FVecAdd::Update(const GameTimer &gt)
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
}

void FVecAdd::Draw(const GameTimer &gt)
{
	return;
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

void FVecAdd::OnMouseDown(WPARAM btnState, int x, int y)
{
	LastMousePos.x = x;
	LastMousePos.y = y;

	SetCapture(MainWnd);
}

void FVecAdd::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void FVecAdd::OnMouseMove(WPARAM btnState, int x, int y)
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

void FVecAdd::OnKeyboardInput(const GameTimer &gt)
{
	if (GetAsyncKeyState('1') & 0x8000)
		bIsWireFrame = true;
	else
		bIsWireFrame = false;
}

void FVecAdd::UpdateCamera(const GameTimer &gt)
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

void FVecAdd::UpdateObjectCBs(const GameTimer &gt)
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

void FVecAdd::UpdateMaterialCBs(const GameTimer& gt)
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

void FVecAdd::BuildBuffers()
{
	// Generate some data.
	std::vector<FData> dataA(gNumDataElements);
	std::vector<FData> dataB(gNumDataElements);
	for (int i = 0; i < gNumDataElements; ++i)
	{
		dataA[i].v1 = XMFLOAT3(i, i, i);
		dataA[i].v2 = XMFLOAT2(i, 0);

		dataB[i].v1 = XMFLOAT3(-i, i, 0.0f);
		dataB[i].v2 = XMFLOAT2(0, -i);
	}

	UINT64 ByteSize = dataA.size() * sizeof(FData);

	// Create some buffers to be used as SRVs.
	InputBufferA = d3dUtil::CreateDefaultBuffer(
		D3DDevice.Get(),
		CommandList.Get(),
		dataA.data(),
		ByteSize,
		InputUploadBufferA);

	InputBufferB = d3dUtil::CreateDefaultBuffer(
		D3DDevice.Get(),
		CommandList.Get(),
		dataB.data(),
		ByteSize,
		InputUploadBufferB);

	// Create the buffer that will be a UAV.
	ThrowIfFailed(D3DDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(ByteSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		nullptr,
		IID_PPV_ARGS(&OutputBuffer)));

	ThrowIfFailed(D3DDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_READBACK),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(ByteSize),
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&ReadBackBuffer)));
}


void FVecAdd::BuildRootSignature()
{
	// Root parameter can be a table, root descriptor or root constants.
	CD3DX12_ROOT_PARAMETER SlotRootParameter[3];

	// Perfomance TIP: Order from most frequent to least frequent.
	SlotRootParameter[0].InitAsShaderResourceView(0);
	SlotRootParameter[1].InitAsShaderResourceView(1);
	SlotRootParameter[2].InitAsUnorderedAccessView(0);

	// A root signature is an array of root parameters.
	CD3DX12_ROOT_SIGNATURE_DESC RootSigDesc(3, SlotRootParameter,
		0, nullptr,
		D3D12_ROOT_SIGNATURE_FLAG_NONE);

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

void FVecAdd::BuildDescriptorHeaps()
{
}

void FVecAdd::BuildShadersAndInputLayout()
{
	Shaders["vecAddCS"] = d3dUtil::CompileShader(L"Shaders\\13_VecAdd\\VecAdd.hlsl", nullptr, "CS", "cs_5_0");
}

void FVecAdd::DoComputeWork()
{
	// Reuse the memory associated with command recording.
	// We can only reset when the associated command lists have finished execution on the GPU.
	ThrowIfFailed(DirectCmdListAlloctor->Reset());

	ThrowIfFailed(CommandList->Reset(DirectCmdListAlloctor.Get(), PSOs["vecAdd"].Get()));
	
	CommandList->SetComputeRootSignature(RootSignature.Get());

	CommandList->SetComputeRootShaderResourceView(0, InputBufferA->GetGPUVirtualAddress());
	CommandList->SetComputeRootShaderResourceView(1, InputBufferB->GetGPUVirtualAddress());
	CommandList->SetComputeRootShaderResourceView(2, OutputBuffer->GetGPUVirtualAddress());
	
	CommandList->Dispatch(1, 1, 1);

	// Schedule to copy the data to the default buffer to the readback buffer.
	CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(OutputBuffer.Get(), 
		D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_SOURCE));

	CommandList->CopyResource(ReadBackBuffer.Get(), OutputBuffer.Get());

	CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(OutputBuffer.Get(), 
		D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_COMMON));

	// Done recording commands.
	ThrowIfFailed(CommandList->Close());
	

	// Add the command list to the queue for execution.
	ID3D12CommandList* CmdsLists[] = { CommandList.Get() };
	CommandQueue->ExecuteCommandLists(_countof(CmdsLists), CmdsLists);

	// Wait for the work to finish.
	FlushCommandQueue();
	
	FData* MappedData = nullptr;
	ThrowIfFailed(ReadBackBuffer->Map(0, nullptr, reinterpret_cast<void**>(&MappedData)));
	
	std::ofstream Out("result.txt");
	
	for (int i = 0; i < gNumDataElements;++i)
	{
		Out << "(" << MappedData[i].v1.x << ", " << MappedData[i].v1.y << ", " << MappedData[i].v1.z <<
			", " << MappedData[i].v2.x << ", " << MappedData[i].v2.y << ")" << std::endl;
	}

	ReadBackBuffer->Unmap(0, nullptr);
}


void FVecAdd::BuildFrameResources()
{
	for (int i = 0; i < gNumFrameResources; ++i)
	{
		FrameResources.push_back(
			std::make_unique<FFrameResource>(
				D3DDevice.Get(),
				1));
	}
}

void FVecAdd::BuildPSOs()
{
	D3D12_COMPUTE_PIPELINE_STATE_DESC  ComputePsoDesc = {};
	ComputePsoDesc.pRootSignature = RootSignature.Get();
	ComputePsoDesc.CS = 
	{
		reinterpret_cast<BYTE*>(Shaders["vecAddCS"]->GetBufferPointer()),
		Shaders["vecAddCS"]->GetBufferSize()
	};
	ComputePsoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	ThrowIfFailed(D3DDevice->CreateComputePipelineState(&ComputePsoDesc, IID_PPV_ARGS(&PSOs["vecAdd"])));
}


int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	try
	{
		FVecAdd App(hInst);
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
