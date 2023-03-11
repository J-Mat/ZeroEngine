#include "PipelineStateObject.h"

namespace Zero
{
	FPipelineStateObject::FPipelineStateObject()
		: m_bValid(false)
		, m_PSODescriptor(FPSODescriptor())
	{
	}
	FPipelineStateObject::FPipelineStateObject(const FPSODescriptor& PSODescriptor)
		: m_bValid(true)
		, m_PSODescriptor(PSODescriptor)
	{
		
	}
	void FPipelineStateObject::SetShader(Ref<FShader> Shader)
	{
		if (m_PSODescriptor.Shader != Shader)
		{
			m_PSODescriptor.Shader = Shader;
			CreatePsoObj();
		}
	}

	void FPipelineStateObject::OnShaderRecompiled(Ref<FShader> Shader)
	{
	}
}
