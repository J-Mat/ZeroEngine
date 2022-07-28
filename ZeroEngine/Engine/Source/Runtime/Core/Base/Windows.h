#pragma once

#include "Core.h"
#include "Core/Events/Event.h"

namespace Zero
{
	struct FWindowsConfig
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;

		FWindowsConfig(const std::string& InTitle = "HEngine",
			uint32_t InWidth = 1600,
			uint32_t InHeight = 900)
			: Title(InTitle), Width(InWidth), Height(InHeight)
		}
		{
	};
	
	class FWindowsBase
	{
	public:
		using EventCallBackFn = std::function<void(FEvent&)>;

		virtual ~FWindowsBase() {}

		virtual void OnUpdate() = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		
		virtual void SetEventCallback(const EventCallBackFn& Callback) = 0;
		virtual void SetVSync(bool bEnabled) = 0;
		virtual bool IsVSync() const = 0;
		
		virtual void* GetNativeWindow() const = 0;
		virtual std::shared_ptr<GraphicsContext> GetGraphicsContext() const = 0;
				
		static std::unique_ptr<FWindowsBase> Create(const FWindowsConfig& Config = FWindowsConfig());
	}
}