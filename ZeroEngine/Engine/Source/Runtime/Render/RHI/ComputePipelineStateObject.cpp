#include "PipelineStateObject.h"
#include "ComputePipelineStateObject.h"

namespace Zero
{
	FComputePipelineStateObject::FComputePipelineStateObject(const FComputePSODescriptor& ComputePSODescriptor)
		: m_ComputePSODescriptor(ComputePSODescriptor)
	{
	}

	void FComputePipelineStateObject::SetShader(Ref<FComputeShader> Shader)
	{
		if (m_ComputePSODescriptor.Shader != Shader)
		{
			m_ComputePSODescriptor.Shader = Shader;
			CreateComputePsoObj();
		}
	}
}
