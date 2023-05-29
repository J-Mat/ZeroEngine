#include "GraphicPipelineStateObject.h"

namespace Zero
{
	FGraphicPipelineStateObject::FGraphicPipelineStateObject()
		: m_bValid(false)
		, m_PSODescriptor(FGraphicPSODescriptor())
	{
	}
	FGraphicPipelineStateObject::FGraphicPipelineStateObject(const FGraphicPSODescriptor& PSODescriptor)
		: m_bValid(true)
		, m_PSODescriptor(PSODescriptor)
	{
		
	}
	void FGraphicPipelineStateObject::SetShader(Ref<FShader> Shader)
	{
		if (m_PSODescriptor.Shader != Shader)
		{
			m_PSODescriptor.Shader = Shader;
			CreateGraphicPSOObj();
		}
	}

	void FGraphicPipelineStateObject::OnShaderRecompiled(Ref<FShader> Shader)
	{
	}
}
