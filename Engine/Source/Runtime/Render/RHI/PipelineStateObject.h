#pragma once
#include "Core.h"
#include "Shader.h"
#include "Render/RenderConfig.h"

namespace Zero
{
	enum EPSOType
	{
		PT_Normal,	
		PT_Depth,
	};
	struct FPSODescriptor
	{
		bool operator==(const FPSODescriptor& Other) const 
		{
			return Other.PSOType == PSOType
				&& Other.Shader == Shader
				&& Other.DepthFunc == DepthFunc
				&& Other.CullMode == CullMode
				&& Other.bDepthEnable == bDepthEnable
				&& Other.TopologyType == TopologyType;
		}
		EPSOType PSOType = EPSOType::PT_Normal;
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
		virtual void Bind(FCommandListHandle CommandListHandle) = 0;
		virtual void CreatePsoObj() = 0;
		const FPSODescriptor& GetPSODescriptor() const {return m_PSODescriptor;}
		void SetShader(Ref<FShader> Shader);
	protected:
		FPSODescriptor m_PSODescriptor;
	};
}