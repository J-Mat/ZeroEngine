#pragma once
#include "PCH.h"


#ifdef ZERO_ENABLE_ASSETS
	#define ZERO_ASSERT(x, ...) {if(!(x)){ZERO_CLIENT_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak();}}
	#define ZERO_CORE_ASSERT(x, ...) {if(!(x)){ZERO_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak();}}
#else
	#define ZERO_ASSERT(x, ...)
	#define ZERO_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)
#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)