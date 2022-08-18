#pragma once

#include "Core.h"
#include "../DX12/Common/DX12Header.h"

namespace Zero
{
	class FDX12CommandQueue;
	class DX12SwapChain;
	class FDX12Texture;
	{
	public:	 
		static const UINT s_BufferCount = 3;

        /**
		* Check to see if tearing is supported.
		*
		* @see https://docs.microsoft.com/en-us/windows/win32/direct3ddxgi/variable-refresh-rate-displays
		*/
        bool IsTearingSupported() const { return m_bTearingSupported; }
		
	private:
		FDX12Device& Device;
		FDX12CommandQueue& m_CommandQueue;
		Ref<FDX12Texture> m_BackBufferTextures
		
		
	protected:
		// Whether or not tearing is supported.
		bool m_bTearingSupported;
	};
}
