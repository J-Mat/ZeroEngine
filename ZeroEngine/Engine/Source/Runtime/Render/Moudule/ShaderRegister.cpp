#include "ShaderRegister.h"
#include "Render/RHI/Shader.h"
#include "Render/RHI/ShaderBinder.h"
#include "Render/RendererAPI.h"

namespace Zero
{
	void FShaderRegister::RegisterDefaultShader(IDevice* Device)
	{
		std::vector<FConstantBufferLayout> CBLayouts =
		{
			FConstantBufferLayout::s_PerObjectConstants,
			// Material
			{
				{EShaderDataType::RGBA, "Color"},
			},
			FConstantBufferLayout::s_PerCameraConstants,
			FConstantBufferLayout::s_PerFrameConstants,
		};
		
		FShaderDesc ShaderDessc = { false, FVertexBufferLayout::s_TestVertexLayout, 1 };
		FRenderer::GraphicFactroy->CreateShader(Device, "Color.hlsl", { CBLayouts }, ShaderDessc);
	}
}
