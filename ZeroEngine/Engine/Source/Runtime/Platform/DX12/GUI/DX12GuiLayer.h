#pragma once

#include "Core.h"
#include "Platform/DX12/Common/DX12Header.h"
#include "GUI/GuiLayer.h"

namespace Zero
{
	class FDX12GuiLayer : public FGuiLayer
	{
		FDX12GuiLayer() = default;
		~FDX12GuiLayer() = default();

		struct GuiAllocation
		{
			D3D12_CPU_DESCRIPTOR_HANDLE m_CpuHandle;
			D3D12_GPU_DESCRIPTOR_HANDLE m_GpuHandle;
		};

	};
}