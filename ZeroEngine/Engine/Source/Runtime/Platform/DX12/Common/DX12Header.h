#pragma once
#include "Utils.h"
#include <wrl.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <D3Dcompiler.h>
#include "d3dx12.h"
#include "../DX12Device.h"

class DxException
{
public:
    DxException() = default;
    DxException(HRESULT hr, const std::wstring& FunctionName, const std::wstring& Filename, int LineNumber) :
    ErrorCode(hr),
    FunctionName(FunctionName),
    Filename(Filename),
    LineNumber(LineNumber)
	{
	}

    std::wstring ToString()const;

    HRESULT ErrorCode = S_OK;
    std::wstring FunctionName;
    std::wstring Filename;
    int LineNumber = -1;
};



#ifndef ThrowIfFailed
#define ThrowIfFailed(x)                                              \
{                                                                     \
    HRESULT hr__ = (x);                                               \
    std::wstring wfn = Zero::Utils::String2WString(__FILE__);               \
    if(FAILED(hr__)) { throw DxException(hr__, L#x, wfn, __LINE__); } \
}
#endif

using namespace Microsoft::WRL;