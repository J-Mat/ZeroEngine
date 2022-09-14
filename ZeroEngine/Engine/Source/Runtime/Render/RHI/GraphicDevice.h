#pragma once
#include "Core.h"

namespace Zero
{
	class FSwapChain;
	class IDevice 
	{
	public:
		virtual void Init() = 0;
		virtual void Resize(uint32_t Width, uint32_t Height) = 0;
		virtual void FlushInitCommandList() = 0;
		virtual void PreInitWorld() = 0;
		virtual Ref<FSwapChain>  GetSwapChain() { return nullptr; }
		virtual void CreateSwapChain(HWND hWnd) = 0;
	};
}