#include "ShaderData.h"

namespace Zero
{
	FBufferElement FBufferElement::s_Pos = { EShaderDataType::Float3, "POSITION" };
	FBufferElement FBufferElement::s_Normal = { EShaderDataType::Float3, "NORMAL", true };
	FBufferElement FBufferElement::s_Tangent = { EShaderDataType::Float3, "TANGENT", true };
	FBufferElement FBufferElement::s_Tex = { EShaderDataType::Float2, "TEXCOORD" };

	FConstantBufferLayout FConstantBufferLayout::s_InvalidContants =
	{
		"Invalid",
		-1,
		{
		},
		false,
	};

	FConstantBufferLayout FConstantBufferLayout::s_PerObjectConstants =
	{
		"cbPerObject",
		0, 
		{
			{EShaderDataType::Mat4, "Model"},
			{EShaderDataType::Int, "ShadingMode"}
		},
		true
	};

	FConstantBufferLayout FConstantBufferLayout::s_PerCameraConstants =
	{
		"cbCameraObject",
		1,
		{
			{EShaderDataType::Mat4, "View"},
			{EShaderDataType::Mat4, "Projection" },
			{EShaderDataType::Mat4, "ProjectionView" },
			{EShaderDataType::Float3, "ViewPos" },
			{EShaderDataType::Float, "gNearZ" },
			{EShaderDataType::Float, "gFarZ" },
		},
		false
	};

	FConstantBufferLayout FConstantBufferLayout::s_GlobalConstants =
	{	
		"cbConstant",
		2,
		{
			{EShaderDataType::Float3,  "DirectLights[0].Color"},
			{EShaderDataType::Float, "DirectLights[0].Intensity"},
			{EShaderDataType::Float3, "DirectLights[0].Direction"},
			{EShaderDataType::Float, "DirectLights[0].Blank"},
			{EShaderDataType::Mat4, "DirectLights[0].ProjectionView"},

			{EShaderDataType::Float3,  "DirectLights[1].Color"},
			{EShaderDataType::Float, "DirectLights[1].Intensity"},
			{EShaderDataType::Float3, "DirectLights[1].Direction"},
			{EShaderDataType::Float, "DirectLights[1].Blank"},
			{EShaderDataType::Mat4, "DirectLights[1].ProjectionView"},

			{EShaderDataType::Float3,  "DirectLights[2].Color"},
			{EShaderDataType::Float, "DirectLights[2].Intensity"},
			{EShaderDataType::Float3, "DirectLights[2].Direction"},
			{EShaderDataType::Float, "DirectLights[2].Blank"},
			{EShaderDataType::Mat4, "DirectLights[2].ProjectionView"},

			{EShaderDataType::Float3,  "DirectLights[3].Color"},
			{EShaderDataType::Float, "DirectLights[3].Intensity"},
			{EShaderDataType::Float3, "DirectLights[3].Direction"},
			{EShaderDataType::Float, "DirectLights[3].Blank"},
			{EShaderDataType::Mat4, "DirectLights[3].ProjectionView"},

			{EShaderDataType::Float3,  "PointLights[0].Color"},
			{EShaderDataType::Float, "PointLights[0].Intensity"},
			{EShaderDataType::Float3, "PointLights[0].LightPos"},
			{EShaderDataType::Float, "PointLights[0].Blank"},

			{EShaderDataType::Float3,  "PointLights[1].Color"},
			{EShaderDataType::Float, "PointLights[1].Intensity"},
			{EShaderDataType::Float3, "PointLights[1].LightPos"},
			{EShaderDataType::Float, "PointLights[1].Blank"},

			{EShaderDataType::Float3,  "PointLights[2].Color"},
			{EShaderDataType::Float, "PointLights[2].Intensity"},
			{EShaderDataType::Float3, "PointLights[2].LightPos"},
			{EShaderDataType::Float, "PointLights[2].Blank"},

			{EShaderDataType::Float3,  "PointLights[3].Color"},
			{EShaderDataType::Float, "PointLights[3].Intensity"},
			{EShaderDataType::Float3, "PointLights[3].LightPos"},
			{EShaderDataType::Float, "PointLights[3].Blank"},

			{EShaderDataType::Int,  "DirectLightNum"},
			{EShaderDataType::Int,  "PointLightNum"},
		},
		false
	};
}