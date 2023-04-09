#include "Input.h"
#include "Render/RendererAPI.h"
#include "Platform/Windows/WinInput.h"
namespace Zero
{
	FInput* FInput::s_Instance = nullptr;

	void FInput::Init()
	{
		switch (Zero::FGraphic::GetRHI())
		{
		case Zero::ERHI::DirectX12: 
			s_Instance = new FWinInput(); 
			break;
		case Zero::ERHI::OpenGL:    
			break; 
		default: 
			break;
		}	
	}
	
	void FInput::Destroy()
	{
		delete s_Instance;
	}
}