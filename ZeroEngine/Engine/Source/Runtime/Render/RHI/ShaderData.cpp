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

		{EShaderDataType::Float3,  "DirectLights[0].Color"},
		{EShaderDataType::Float, "DirectLights[0].Intensity"},
		{EShaderDataType::Mat4, "DirectLights[0].ProjView"},
		{EShaderDataType::Float3, "DirectLights[0].Direction"},
		


		{EShaderDataType::Float3,  "DirectLights[1].Color"},
		{EShaderDataType::Float, "DirectLights[1].Intensity"},
		{EShaderDataType::Mat4, "DirectLights[1].ProjView"},
		{EShaderDataType::Float3, "DirectLights[1].Direction"},

		{EShaderDataType::Float3,  "DirectLights[2].Color"},
		{EShaderDataType::Float, "DirectLights[2].Intensity"},
		{EShaderDataType::Mat4, "DirectLights[2].ProjView"},
		{EShaderDataType::Float3, "DirectLights[2].Direction"},

		{EShaderDataType::Float3,  "DirectLights[3].Color"},
		{EShaderDataType::Float, "DirectLights[3].Intensity"},
		{EShaderDataType::Mat4, "DirectLights[3].ProjView"},
		{EShaderDataType::Float3, "DirectLights[3].Direction"},

		{EShaderDataType::Int,  "DirectLightNum"},
	};

	void FConstantBufferLayout::GenderateConstBufferLayoutByShader(std::vector<FConstantBufferLayout>& Layouts,
		const std::map<uint32_t, FShaderCBVParameter>& CBVParameters)
	{
		for (auto Iter : CBVParameters)
		{
			FConstantBufferLayout ConstantBufferLayout;
			for (const FCBVariableItem& CBVariableItem : Iter.second.VariableList)
			{
				FBufferElement Element = { CBVariableItem.ShaderDataType, CBVariableItem.VariableName };
				ConstantBufferLayout.m_Elements.push_back(Element);
			}
			ConstantBufferLayout.CalculateOffsetsAndStride();
			Layouts.push_back(ConstantBufferLayout);
		}
	}
}