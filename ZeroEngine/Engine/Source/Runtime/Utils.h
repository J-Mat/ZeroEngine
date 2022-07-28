#pragma once
#include "Core.h"

namespace Zero
{
	class FUtils
	{
		inline std::string WString2String(const std::wstring& ws)
		{
			std::string strLocale = setlocale(LC_ALL, "");
			const wchar_t* wchSrc = ws.c_str();
			size_t nDestSize = wcstombs(NULL, wchSrc, 0) + 1;
			char* chDest = new char[nDestSize];
			memset(chDest, 0, nDestSize);
			wcstombs(chDest, wchSrc, nDestSize);
			std::string strResult = chDest;
			delete[]chDest;
			setlocale(LC_ALL, strLocale.c_str());
			return strResult;
		}

		inline UINT CalcConstantBufferByteSize(UINT ByteSize)
		{
    		// Constant buffers must be a multiple of the minimum hardware 
    		// allocation size (usually 256 bytes). So round up to nearest 
    		// multiple of 256. We do this by adding 255 and then masking off 
    		// the lower 2 bytes which store all bits < 256. 
    		// Example: Suppose ByteSize = 300. 
    		// (300 + 255) & ˜255 
    		// 555 & ˜255 
    		// 0x022B & ˜0x00ff 
    		// 0x022B & 0xff00 
    		// 0x0200 
    		// 512 
    		return (ByteSize + 255) & ~255;
		}
	};
}