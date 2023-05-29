#pragma once
#include "Core.h"
#include "Shader/Shader.h"
#include "Render/RenderConfig.h"

namespace Zero
{
	struct FComputePSODescriptor
	{
		bool operator==(const FComputePSODescriptor& Other) const
		{
			return Other.Shader == Shader &&
				Other.BlockSize_X == BlockSize_X &&
				Other.BlockSize_Y == BlockSize_Y &&
				Other.BlockSize_Z == BlockSize_Z;
		}
		Ref<FShader> Shader = nullptr;
		uint32_t BlockSize_X = 8;
		uint32_t BlockSize_Y = 8;
		uint32_t BlockSize_Z = 8;
	};

	class FComputePipelineStateObject
	{
	public:
		FComputePipelineStateObject(const FComputePSODescriptor& ComputePSODescriptor);
		virtual ~FComputePipelineStateObject() = default;
		virtual void Bind() = 0;
		virtual void CreateComputePsoObj() = 0;
		const FComputePSODescriptor& GetPSODescriptor() const {return m_ComputePSODescriptor;}
		void SetShader(Ref<FShader> Shader);
	protected:
		FComputePSODescriptor m_ComputePSODescriptor;
	};
}