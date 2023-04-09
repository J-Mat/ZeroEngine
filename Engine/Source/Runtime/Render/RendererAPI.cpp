#include "RendererAPI.h"


namespace Zero
{
	Scope<IGraphicFactroy> FGraphic::GraphicFactroy = nullptr;
	ERHI FGraphic::RHI = ERHI::DirectX12;
	ERayTracerAPI FGraphic::RayTracer = ERayTracerAPI::DXR;
	std::vector<Ref<IDevice>> FGraphic::Devices;

	void FGraphic::InitAPI()
	{
	}

	void FGraphic::SetRHI(ERHI InRHI)
	{	
		RHI = InRHI;

		switch (RHI)
		{
			case ERHI::DirectX12: GraphicFactroy = CreateScope<FDX12Factory>();
			case ERHI::OpenGL:    break; 
			default: break;
		}

		
	}

}