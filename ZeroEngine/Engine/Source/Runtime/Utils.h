#pragma once
#include "Core.h"


// Set the name of an std::thread.
// Useful for debugging.
const DWORD MS_VC_EXCEPTION = 0x406D1388;
#pragma pack( push, 8 )
typedef struct tagTHREADNAME_INFO
{
	DWORD  dwType;      // Must be 0x1000.
	LPCSTR szName;      // Pointer to name (in user addr space).
	DWORD  dwThreadID;  // Thread ID (-1=caller thread).
	DWORD  dwFlags;     // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack( pop )

namespace Zero
{
	namespace Utils
	{
		inline void SetThreadName(std::thread& thread, const char* threadName)
		{
			THREADNAME_INFO info;
			info.dwType = 0x1000;
			info.szName = threadName;
			info.dwThreadID = ::GetThreadId(reinterpret_cast<HANDLE>(thread.native_handle()));
			info.dwFlags = 0;

			__try
			{
				::RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
			}
		}

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