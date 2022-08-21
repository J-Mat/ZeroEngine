#pragma once
#include "Utils.h"
#include <wrl.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <D3Dcompiler.h>
#include "d3dx12.h"
#include <comdef.h> 
#include <cstdint>
#include <codecvt>

namespace Zero
{ 
	inline void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			_com_error err(hr);
			OutputDebugString(err.ErrorMessage());
			std::wstring test = err.ErrorMessage();
			throw std::exception(Utils::WString2String(test).c_str());
		}
	}
}

using namespace Microsoft::WRL;