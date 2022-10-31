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
		{EShaderDataType::Int, "DirectionalLightNum"},
		{EShaderDataType::Int, "PointLightNum"},

		{EShaderDataType::Mat4, "DirectionalLights[0].ProjView"},
		{EShaderDataType::Float3, "DirectionalLights[0].Direction"},
		{EShaderDataType::Float,  "DirectionalLights[0].Intensity"},
		{EShaderDataType::Float3, "DirectionalLights[0].Color"},

		{EShaderDataType::Mat4, "DirectionalLights[1].ProjView"},
		{EShaderDataType::Float3, "DirectionalLights[1].Direction"},
		{EShaderDataType::Float,  "DirectionalLights[1].Intensity"},
		{EShaderDataType::Float3, "DirectionalLights[1].Color"},

		{EShaderDataType::Mat4, "DirectionalLights[2].ProjView"},
		{EShaderDataType::Float3, "DirectionalLights[2].Direction"},
		{EShaderDataType::Float,  "DirectionalLights[2].Intensity"},
		{EShaderDataType::Float3, "DirectionalLights[2].Color"},

		{EShaderDataType::Mat4, "DirectionalLights[3].ProjView"},
		{EShaderDataType::Float3, "DirectionalLights[3].Direction"},
		{EShaderDataType::Float,  "DirectionalLights[3].Intensity"},
		{EShaderDataType::Float3, "DirectionalLights[3].Color"},
		
	};
}