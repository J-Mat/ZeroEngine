#include "VertexBuffer.h"

namespace Zero
{
	FVertexBufferLayout FVertexBufferLayout::s_DefaultVertexLayout = 
	{
		{ EShaderDataType::Float3, "POSITION" },
		{ EShaderDataType::Float3, "NORMAL", true },
		{ EShaderDataType::Float3, "TANGENT", true },
		{ EShaderDataType::Float2, "TEXCOORD" }
	};

	FVertexBufferLayout FVertexBufferLayout::s_SkyboxVertexLayout = 
	{
		{ EShaderDataType::Float3, "POSITION" },
		{ EShaderDataType::Float2, "TEXCOORD" }
	};
}