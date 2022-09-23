#include "DX12GuiLayer.h"
#include "Render/RendererAPI.h"
#include "Platform/DX12/DX12Device.h"
#include "Core/Framework/Application.h"
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_win32.h"
#include "imgui/backends/imgui_impl_dx12.h"
#include "../DX12CommandList.h"
#include "../MemoryManage/DescriptorAllocation.h"

namespace  Zero
{
	static int const NUM_FRAMES_IN_FLIGHT = 3;

	FDX12GuiLayer::~FDX12GuiLayer()
	{
		PlatformDestroy();
	}

	void FDX12GuiLayer::PlatformInit()
	{
		m_Device = static_cast<FDX12Device*>(FRenderer::GetDevice().get())->AsShared();
		m_Device->CreateGuiDescHeap();
		FLightDescrptorAllocation Allocation = m_Device->AllocateGuiDescritor();
		ImGui_ImplWin32_Init(FApplication::Get().GetWindow().GetNativeWindow());
		ImGui_ImplDX12_Init(
			m_Device->GetDevice(), 
			NUM_FRAMES_IN_FLIGHT, 
			DXGI_FORMAT_R8G8B8A8_UNORM,
			m_Device->GetGuiDescHeap().Get(),
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
		auto  D3dCommandList = m_Device->GetRenderCommandList()->GetD3D12CommandList();
		ID3D12DescriptorHeap* Heap = m_Device->GetGuiDescHeap().Get();
		D3dCommandList->SetDescriptorHeaps(1, &Heap);
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), D3dCommandList.Get());
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
		auto*  D3dCommandList = m_Device->GetRenderCommandList()->GetD3D12CommandList().Get();
		// Update and Render additional Platform Windows
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault(NULL, (void*)D3dCommandList);
		}
	}
}
