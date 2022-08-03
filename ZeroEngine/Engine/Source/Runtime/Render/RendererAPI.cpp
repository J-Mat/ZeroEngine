#include "RendererAPI.h"
#include "Platform/DX12/DX12IndexBuffer.h"


namespace Zero
{
	void FRenderer::InitAPI()
	{
	}

	static void SetRHI(ERHI InRHI)
	{	
		RHI = InRHI;

		switch (RHI)
		{
		case ERHI::DirectX12: GraphicFactroy = CreateScope<FDX12Factory>();
		case ERHI::OpenGL:    break; 
		default: break;
		}

		
	}


	Scope<IGraphicFactroy> FRenderer::GraphicFactroy = nullptr;
	ERHI FRenderer::RHI = ERHI::DirectX12;
	ERayTracerAPI FRenderer::RayTracer = ERayTracerAPI::DXR;
	std::function<FIndexBuffer*(unsigned int*, uint32_t)> CreateIndexBufferFuncs = {}; 
}