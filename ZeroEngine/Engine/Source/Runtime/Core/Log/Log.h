#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
namespace Zero
{
class FLog
{
public:
	static void Init();
	inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
private:
	static std::shared_ptr<spdlog::logger> s_CoreLogger;
	static std::shared_ptr<spdlog::logger> s_ClientLogger;
};
}

// Core log macros
#define CORE_LOG_LOG_TRACE(...)		::Zero::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define CORE_LOG_INFO(...)		::Zero::Log::GetCoreLogger()->info(__VA_ARGS__)
#define CORE_LOG_WARN(...)		::Zero::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define CORE_LOG_ERROR(...)		::Zero::Log::GetCoreLogger()->error(__VA_ARGS__)
#define CORE_LOG_FATAL(...)		::Zero::Log::GetCoreLogger()->fatal(__VA_ARGS__)

// Client log macros
#define CLIENT_LOG_TRACE(...)			::Zero::Log::GetClientLogger()->trace(__VA_ARGS__)
#define CLIENT_LOG_INFO(...)			::Zero::Log::GetClientLogger()->info(__VA_ARGS__)
#define CLIENT_LOG_WARN(...)			::Zero::Log::GetClientLogger()->warn(__VA_ARGS__)
#define CLIENT_LOG_ERROR(...)			::Zero::Log::GetClientLogger()->error(__VA_ARGS__)
#define CLIENT_LOG_FATAL(...)			::Zero::Log::GetClientLogger()->fatal(__VA_ARGS__)