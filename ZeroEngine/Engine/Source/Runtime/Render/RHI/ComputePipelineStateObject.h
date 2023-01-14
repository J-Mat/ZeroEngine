#pragma once
#include "Core.h"
#include "Shader.h"
#include "Render/RenderConfig.h"

namespace Zero
{
	struct FComputePSODescriptor
	{
		bool operator==(const FComputePSODescriptor& Other) const
		{
			return Other.Shader == Shader;
		}
		Ref<FComputeShader> Shader = nullptr;
	};

	class FComputePipelineStateObject
	{
	public:
		FComputePipelineStateObject(const FComputePSODescriptor& ComputePSODescriptor);
		virtual ~FComputePipelineStateObject() = default;
		virtual void Bind() = 0;
		virtual void CreateComputePsoObj() = 0;
		const FComputePSODescriptor& GetPSODescriptor() const {return m_ComputePSODescriptor;}
		void SetShader(Ref<FComputeShader> Shader);
	protected:
		FComputePSODescriptor m_ComputePSODescriptor;
	};
}