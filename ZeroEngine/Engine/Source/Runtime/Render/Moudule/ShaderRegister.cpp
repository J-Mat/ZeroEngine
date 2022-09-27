#include "ShaderRegister.h"
#include "Render/RHI/Shader.h"
#include "Render/RHI/ShaderBinder.h"
#include "Render/RendererAPI.h"

namespace Zero
{
	void FShaderRegister::RegisterDefaultShader()
	{
		std::vector<FConstantBufferLayout> CBLayouts =
		{
			FConstantBufferLayout::s_PerObjectConstants,
			FConstantBufferLayout::s_PerCameraConstants,
			// Material
			{
				{EShaderDataType::RGBA, "Base"},
			},
			FConstantBufferLayout::s_PerFrameConstants
		};
		
		FShaderResourceLayout ResourceLayout =
		{
			{EShaderResourceType::Texture2D, "gDiffuseMap"}
		};

		FShaderDesc ShaderDessc = { false, FVertexBufferLayout::s_DefaultVertexLayout, 1 };
		
		FRenderer::GraphicFactroy->CreateShader("Color.hlsl", {CBLayouts, ResourceLayout}, ShaderDessc);
	}
}
