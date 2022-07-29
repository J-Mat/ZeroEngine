#pragma once
#include "Core.h"

namespace Zero
{
	namespace Utils
	{
		static inline std::string WString2String(const std::wstring& Input)
		{
			std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
			return converter.to_bytes(Input);
		}

		static inline std::wstring String2WString(const std::string& Input)
		{
			std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
			return converter.from_bytes(Input);
		}
	};
}