#include "Windows.h"
#include "Render/RendererAPI.h"
#include "Platform/Windows/WinWindow.h"
namespace Zero
{

	Ref<FWindows> Zero::FWindows::Create(const FWindowsConfig& Config)
	{
		switch (Zero::FRenderer::GetRHI())
		{
		case Zero::ERHI::DirectX12:
			return CreateRef<FWinWindow>(Config);
		case Zero::ERHI::OpenGL:
			break;
		default:
			break;
		}
		return nullptr;
	}
}
