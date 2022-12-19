#pragma once
#include "Core.h"
#include "Shader.h"
#include "Render/RenderConfig.h"

namespace Zero
{
	struct FPSODescriptor
	{
		bool operator==(const FPSODescriptor& Other) const 
		{
			return Other.Shader == Shader
				&& Other.DepthFunc == DepthFunc
				&& Other.CullMode == CullMode
				&& Other.bDepthEnable == bDepthEnable
				&& Other.TopologyType == TopologyType;
		}
		Ref<FShader> Shader = nullptr;
		bool _4xMsaaState = false;
		UINT _4xMsaaQuality = 0;
		EPrimitiveTopologyType TopologyType = EPrimitiveTopologyType::PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		EComparisonFunc DepthFunc = EComparisonFunc::LESS;
		bool bDepthEnable = true;
		ECullMode CullMode = ECullMode::CULL_MODE_BACK;
	};

	class FPipelineStateObject
	{
	public:
		FPipelineStateObject(const FPSODescriptor& PSODescriptor);
		virtual ~FPipelineStateObject() = default;
		virtual void Bind() = 0;
		virtual void CreatePsoObj() = 0;
		const FPSODescriptor& GetPSODescriptor() const {return m_PSODescriptor;}
		void SetShader(Ref<FShader> Shader);
	protected:
		FPSODescriptor m_PSODescriptor;
	};
}