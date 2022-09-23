#pragma once

#include "Core.h"
#include "Platform/DX12/Common/DX12Header.h"
#include "GUI/GuiLayer.h"

namespace Zero
{
	class FDX12Device;
	class FDX12GuiLayer : public FGuiLayer
	{
	public:
		FDX12GuiLayer() = default;
		~FDX12GuiLayer();
	protected:
		virtual void PlatformInit() override;
		virtual void NewFrameBegin() override;
		virtual void DrawCall() override;
		virtual void PlatformDestroy() override;
		virtual void PostDraw() override;
	private:
		Ref<FDX12Device> m_Device;
	};
}