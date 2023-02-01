#include "RendererAPI.h"
#include "Platform/DX12/DX12IndexBuffer.h"


namespace Zero
{
	Scope<IGraphicFactroy> FRenderer::GraphicFactroy = nullptr;
	ERHI FRenderer::RHI = ERHI::DirectX12;
	ERayTracerAPI FRenderer::RayTracer = ERayTracerAPI::DXR;
	std::vector<Ref<IDevice>> FRenderer::Devices;

	void FRenderer::InitAPI()
	{
	}

	void FRenderer::SetRHI(ERHI InRHI)
	{	
		RHI = InRHI;

		switch (RHI)
		{
		case ERHI::DirectX12: GraphicFactroy = CreateScope<FDX12Factory>();
		case ERHI::OpenGL:    break; 
		default: break;
		}

		
	}


	std::function<FIndexBuffer*(unsigned int*, uint32_t)> CreateIndexBufferFuncs = {}; 
}