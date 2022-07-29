#pragma once
#include "PCH.h"

#define ZERO_ENABLE_ASSETS

#ifdef ZERO_ENABLE_ASSETS
	#define CLIENT_ASSERT(x, ...) {if(!(x)){CLIENT_LOG_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak();}}
	#define CORE_ASSERT(x, ...) {if(!(x)){CORE_LOG_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak();}}
#else
	#define CLIENT_ASSERT(x, ...)
	#define CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)
#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

namespace Zero
{
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
	
}