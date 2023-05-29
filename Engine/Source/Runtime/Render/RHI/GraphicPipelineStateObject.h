#pragma once
#include "Core.h"
#include "./Shader/Shader.h"
#include "Render/RenderConfig.h"

namespace Zero
{
	enum EPSOType
	{
		PT_Normal,	
		PT_Depth,
	};
	struct FGraphicPSODescriptor
	{
		bool operator==(const FGraphicPSODescriptor& Other) const 
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

	class FGraphicPipelineStateObject
	{
	public:
		FGraphicPipelineStateObject();
		FGraphicPipelineStateObject(const FGraphicPSODescriptor& PSODescriptor);
		virtual ~FGraphicPipelineStateObject() = default;
		virtual void Bind(FCommandListHandle CommandListHandle) = 0;
		virtual void CreateGraphicPSOObj() = 0;
		const FGraphicPSODescriptor& GetPSODescriptor() const {return m_PSODescriptor;}
		void SetShader(Ref<FShader> Shader);
		void OnShaderRecompiled(Ref<FShader> Shader);
		bool IsValid() { return m_bValid; }
	protected:
		bool m_bValid;
		FGraphicPSODescriptor m_PSODescriptor;
	};
}