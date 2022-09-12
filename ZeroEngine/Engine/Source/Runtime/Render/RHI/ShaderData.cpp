#include "ShaderData.h"

namespace Zero
{
	FBufferElement FBufferElement::s_Pos = { EShaderDataType::Float3, "PosH" };
	FBufferElement FBufferElement::s_Color = { EShaderDataType::RGBA, "Color" };

	FConstantBufferLayout FConstantBufferLayout::s_PerObjectConstants =
	{
		{EShaderDataType::Mat4, "Model"},
	};

	FConstantBufferLayout FConstantBufferLayout::s_PerCameraConstants =
	{
		{EShaderDataType::Mat4, "View"},
		{EShaderDataType::Mat4, "Projection" },
		{EShaderDataType::Mat4, "ProjectionView" },
		{EShaderDataType::Mat4, "PreviousPV" },
		{EShaderDataType::Mat4, "CurrentPV" },
		{EShaderDataType::Float4, "ViewPos" },
		{EShaderDataType::Float4, "ZNearFar" },
	};

	FConstantBufferLayout FConstantBufferLayout::s_PerFrameConstants =
	{
		{EShaderDataType::Int, "DirectionalLightNum"},
		{EShaderDataType::Int, "PointLightNum"},

		{EShaderDataType::Mat4, "directionalLights[0].projview"},
		{EShaderDataType::Float3, "directionalLights[0].direction"},
		{EShaderDataType::Float,  "directionalLights[0].intensity"},
		{EShaderDataType::Float3, "directionalLights[0].color"},

		{EShaderDataType::Mat4, "directionalLights[1].projview"},
		{EShaderDataType::Float3, "directionalLights[1].direction"},
		{EShaderDataType::Float,  "directionalLights[1].intensity"},
		{EShaderDataType::Float3, "directionalLights[1].color"},

		{EShaderDataType::Mat4, "directionalLights[2].projview"},
		{EShaderDataType::Float3, "directionalLights[2].direction"},
		{EShaderDataType::Float,  "directionalLights[2].intensity"},
		{EShaderDataType::Float3, "directionalLights[2].color"},

		{EShaderDataType::Mat4, "directionalLights[3].projview"},
		{EShaderDataType::Float3, "directionalLights[3].direction"},
		{EShaderDataType::Float,  "directionalLights[3].intensity"},
		{EShaderDataType::Float3, "directionalLights[3].color"},
	};
}