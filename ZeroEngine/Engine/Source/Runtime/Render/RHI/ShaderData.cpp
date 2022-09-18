#include "ShaderData.h"

namespace Zero
{
	FBufferElement FBufferElement::s_Pos = { EShaderDataType::Float3, "POSITION" };
	FBufferElement FBufferElement::s_Normal = { EShaderDataType::Float3, "NORMAL", true };
	FBufferElement FBufferElement::s_Tangent = { EShaderDataType::Float3, "TANGENT", true }; 
	FBufferElement FBufferElement::s_Tex = { EShaderDataType::Float2, "TEXCOORD" }; 

	FConstantBufferLayout FConstantBufferLayout::s_PerObjectConstants =
	{
		{EShaderDataType::Mat4, "Model"},
	};

	FConstantBufferLayout FConstantBufferLayout::s_PerCameraConstants =
	{
		{EShaderDataType::Mat4, "View"},
		{EShaderDataType::Mat4, "Projection" },
		{EShaderDataType::Mat4, "ProjectionView" },
		{EShaderDataType::Float3, "ViewPos" },
	};

	FConstantBufferLayout FConstantBufferLayout::s_PerFrameConstants =
	{
		{EShaderDataType::RGBA, "Color"},
	};
}