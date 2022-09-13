#include "ShaderData.h"

namespace Zero
{
	FBufferElement FBufferElement::s_Pos = { EShaderDataType::Float3, "POSITION" };
	FBufferElement FBufferElement::s_Color = { EShaderDataType::RGBA, "COLOR" };

	FConstantBufferLayout FConstantBufferLayout::s_PerObjectConstants =
	{
		{EShaderDataType::Mat4, "Model"},
	};

	FConstantBufferLayout FConstantBufferLayout::s_PerCameraConstants =
	{
		{EShaderDataType::Mat4, "View"},
		{EShaderDataType::Mat4, "Projection" },
		{EShaderDataType::Mat4, "ProjectionView" },
	};

	FConstantBufferLayout FConstantBufferLayout::s_PerFrameConstants =
	{
		{EShaderDataType::Float, "Test"},
	};
}