#pragma once
#include "Core.h"
#include "Render/RenderConfig.h"

namespace Zero
{
	class FSwapChain;
	class FCommandList;
	class IDevice 
	{
	public:
		virtual void Init() = 0;
		virtual void Resize(uint32_t Width, uint32_t Height) = 0;
		virtual void BeginFrame() = 0;
		virtual void PreInitWorld() = 0;
		virtual void FlushInitCommandList() = 0;
		virtual FCommandListHandle GenerateCommanList(ERenderPassType RenderPassType = ERenderPassType::Graphics) = 0;
		virtual FCommandListHandle GetSingleThreadCommadList() = 0;
		virtual void Flush() = 0;
		virtual Ref<FSwapChain>  GetSwapChain() { return nullptr; }
		virtual void CreateSwapChain(HWND hWnd) = 0;
	};
}