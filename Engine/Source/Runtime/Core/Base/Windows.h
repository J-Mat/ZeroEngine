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
		HINSTANCE hAppInst;
		bool bScreenMaxSize = true;

		FWindowsConfig(HINSTANCE InhAppInst, const std::string& InTitle = "ZeroEngine",
			uint32_t InWidth = 1600,
			uint32_t InHeight = 900,
			bool _bScreenMaxSize = true)
			: hAppInst(InhAppInst), Title(InTitle), Width(InWidth), Height(InHeight), bScreenMaxSize(_bScreenMaxSize)
		{}
	};
	
	class IDevice;
	class FWindow
	{
	public:
		using EventCallBackFn = std::function<void(FEvent&)>;

		virtual ~FWindow() {}

		virtual void OnUpdate() = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		
		virtual void SetEventCallback(const EventCallBackFn& Callback) = 0;
		virtual void SetVSync(bool bEnabled) = 0;
		virtual bool IsVSync() const = 0;
		
		virtual void* GetNativeWindow() const = 0;
		// virtual std::shared_ptr<GraphicsContext> GetGraphicsContext() const = 0;
		
		Ref<IDevice> GetDevice() { return m_Device; }

	protected:
		uint32_t m_DeviceIndex = -1;
		Ref<IDevice> m_Device;
		
	};
}