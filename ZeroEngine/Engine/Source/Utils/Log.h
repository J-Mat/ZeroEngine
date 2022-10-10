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
	inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
private:
	static std::shared_ptr<spdlog::logger> s_CoreLogger;
	static std::shared_ptr<spdlog::logger> s_ClientLogger;
};
}

// Core log macros
#define CORE_LOG_TRACE(...)		Zero::FLog::GetCoreLogger()->trace(__VA_ARGS__)
#define CORE_LOG_INFO(...)		Zero::FLog::GetCoreLogger()->info(__VA_ARGS__)
#define CORE_LOG_WARN(...)		Zero::FLog::GetCoreLogger()->warn(__VA_ARGS__)
#define CORE_LOG_ERROR(...)		Zero::FLog::GetCoreLogger()->error(__VA_ARGS__)
#define CORE_LOG_FATAL(...)		Zero::FLog::GetCoreLogger()->fatal(__VA_ARGS__)

// Client log macros
#define CLIENT_LOG_TRACE(...)			Zero::FLog::GetClientLogger()->trace(__VA_ARGS__)
#define CLIENT_LOG_INFO(...)			Zero::FLog::GetClientLogger()->info(__VA_ARGS__)
#define CLIENT_LOG_WARN(...)			Zero::FLog::GetClientLogger()->warn(__VA_ARGS__)
#define CLIENT_LOG_ERROR(...)			Zero::FLog::GetClientLogger()->error(__VA_ARGS__)
#define CLIENT_LOG_FATAL(...)			Zero::FLog::GetClientLogger()->fatal(__VA_ARGS__)