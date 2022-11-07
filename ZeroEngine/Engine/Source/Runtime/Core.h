#pragma once
#include "PCH.h"
#include "Log.h"


#define EDITOR_MODE 1

#define _KB(x) (x * 1024)
#define _MB(x) (x * 1024 * 1024)

#define _64KB _KB(64)
#define _1MB _MB(1)
#define _2MB _MB(2)
#define _4MB _MB(4)
#define _8MB _MB(8)
#define _16MB _MB(16)
#define _32MB _MB(32)
#define _64MB _MB(64)
#define _128MB _MB(128)
#define _256MB _MB(256)


#define BIT(x) (1 << x)
#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)
#define TO_VOIDPTR_FROM_DATA(Value) ((void*)&Value)
#define TO_VOID_PTR_FROM_PTR(Ptr) ((void*)Ptr)

#define FORWARD_STAGE "ForwardStage"

namespace Zero
{
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
	}

	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
	using FJsonObj = nlohmann::json;
	using FTextureHandle = std::string;
	using FMaterialHandle = std::string;
}