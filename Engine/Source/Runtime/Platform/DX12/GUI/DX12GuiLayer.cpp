#include "DX12GuiLayer.h"
#include "Render/RendererAPI.h"
#include "Platform/DX12/DX12Device.h"
#include "Core/Framework/Application.h"
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_win32.h"
#include "imgui/backends/imgui_impl_dx12.h"
#include "../DX12CommandList.h"
#include "../MemoryManage/Descriptor/DescriptorAllocation.h"

namespace  Zero
{
	static int const NUM_FRAMES_IN_FLIGHT = 3;

	FDX12GuiLayer::~FDX12GuiLayer()
	{
		PlatformDestroy();
	}

	void FDX12GuiLayer::PlatformInit()
	{
		FLightDescrptorAllocation Allocation = FDX12Device::Get()->AllocateGuiDescritor();
		ImGui_ImplWin32_Init(FApplication::Get().GetWindow().GetNativeWindow());
		ImGui_ImplDX12_Init(
			FDX12Device::Get()->GetDevice(),
			NUM_FRAMES_IN_FLIGHT, 
			DXGI_FORMAT_R8G8B8A8_UNORM,
			FDX12Device::Get()->GetGuiDescHeap().Get(),
			Allocation.CpuHandle,
			Allocation.GpuHandle
		);
	}

	void FDX12GuiLayer::NewFrameBegin()
	{
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
	}

	void FDX12GuiLayer::DrawCall()
	{
		FCommandListHandle Handle = FDX12Device::Get()->GetSingleThreadCommadList();
		m_GuiCommandList = FDX12Device::Get()->GetCommanList(Handle);
		ID3D12DescriptorHeap* Heap = FDX12Device::Get()->GetGuiDescHeap().Get();
		m_GuiCommandList->GetD3D12CommandList()->SetDescriptorHeaps(1, &Heap);
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), m_GuiCommandList->GetD3D12CommandList().Get());
	}

	void FDX12GuiLayer::PlatformDestroy()
	{
		ImGui_ImplDX12_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	void FDX12GuiLayer::PostDraw()
	{
		ImGuiIO& io = ImGui::GetIO();
		// Update and Render additional Platform Windows
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault(NULL, (void*)(m_GuiCommandList->GetD3D12CommandList().Get()));
		}
	}
}
