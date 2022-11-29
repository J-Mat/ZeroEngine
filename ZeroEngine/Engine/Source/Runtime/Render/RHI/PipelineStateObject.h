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
				&& Other.TopologyType == TopologyType;
		}
		Ref<IShader> Shader = nullptr;
		bool _4xMsaaState = false;
		UINT _4xMsaaQuality = 0;
		EPrimitiveTopologyType TopologyType = EPrimitiveTopologyType::PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		EComparisonFunc DepthFunc = EComparisonFunc::LESS;
		ECullMode CullMode = ECullMode::CULL_MODE_BACK;
	};

	class FPipelineStateObject
	{
	public:
		FPipelineStateObject(const FPSODescriptor& PSODescriptor);
		virtual ~FPipelineStateObject() = default;
		virtual void Bind() = 0;
		const FPSODescriptor& GetPSODescriptor() {	return m_PSODescriptor;}
	private:
		FPSODescriptor m_PSODescriptor;
	};
}