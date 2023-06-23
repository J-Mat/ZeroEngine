#pragma once
#include "Core.h"
#include "Math/ZMath.h"

namespace Zero
{

	enum class ERenderLayer
	{
		 Opaque,
		 Transparent,
		 Light,
		 Skybox,
		 Shadow,
		 Unknown,
	};

	enum class EPiplineStateMode
	{
		Dependent,
		AllSpecific,
		Only,
	};

	struct FRenderParams
	{
		ERenderLayer RenderLayer;
		EPiplineStateMode PiplineStateMode = EPiplineStateMode::Dependent;
		uint32_t PsoID = ZERO_INVALID_ID;
	};
	struct FDispatchComputeParams
	{
		uint32_t PsoID = ZERO_INVALID_ID;
		uint32_t ThreadNumX;
		uint32_t ThreadNumY;
		uint32_t ThreadNumZ;
	};

	#define RENDER_STAGE_FORWARD "ForwardLit"
	#define RENDER_STAGE_SHADOWMAP "ShadowMap"
	#define RENDER_STAGE_SHADOWMAP_DEBUG "ShadowMapDebug"

	#define IBL_BRDF_LUT "IBL_BRDF_LUT"
	
	
	#define GENERATE_MIP_SHADER "Shader\\Compute\\GenerateMips_CS.hlsl"
	#define GENERATE_MIP_SHADER_TEST "Shader\\Compute\\Test_CS.hlsl"



	enum class ERHI
	{
		DirectX12,
		OpenGL,
		RHI_NUM
	};

	enum ERenderPassType
	{
		Graphics,
		Compute,
		Copy,
		ComputeAsync,
		TypeNum,
	};

	enum ERenderPassRTType
	{
		Texuture2D,
		TexutureCube,
		RenderPass_None,
	};

	enum class ERayTracerAPI
	{
		CpuSoftware,
		DXR,
	};

	enum class EComparisonFunc
	{
		NEVER,
		LESS,
		EQUAL,
		LESS_EQUAL,
		GREATER,
		NOT_EQUAL,
		GREATER_EQUAL,
		ALWAYS
	};
	
	enum EPrimitiveTopologyType
	{
		PRIMITIVE_TOPOLOGY_TYPE_POINT,
		PRIMITIVE_TOPOLOGY_TYPE_LINE,
		PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
	};

	enum ECullMode
	{
		CULL_MODE_NONE,
		CULL_MODE_FRONT,
		CULL_MODE_BACK,
	};

	struct FCommandListHandle
	{
		uint32_t CommandListIndex = static_cast<uint32_t>(-1);
		ERenderPassType RenderPassType = ERenderPassType::Graphics;
	};

	namespace EShaderID
	{
		enum
		{
			Error,
			SkyBox,
			ForwardLit,
			DirectLight,
			PointLight,
			IBLIrradiance,
			IBLPrefilter,
			Shadow,
			ShadowMap,
			ShadowDebug,
			DirectLightShadowMap,
			PointLightShadowMap,
			PrefixSumTex,
			ShaderCount,
		};
	}

	namespace EGraphicPsoID
	{
		enum
		{
			GlobalPso,
			Skybox,
			DirectLightShadowMap,
			PointLightShadowMap,
			ShadowDebug,
			ForwadLit,
			DirectLight,
			PointLight,
			GenerateMips,
			IBLIrradiance,
			IBLPrefilter,
			PSOCount,
		};
	}
	namespace EComputePsoID
	{
		enum
		{
			PrefixSumTex,
			TestPostProcess,
			PSOCount,
		};
	}
}