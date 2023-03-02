#include "PipelineStateObject.h"

namespace Zero
{
	FPipelineStateObject::FPipelineStateObject(const FPSODescriptor& PSODescriptor)
		: m_PSODescriptor(PSODescriptor)
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
