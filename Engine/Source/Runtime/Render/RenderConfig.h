#pragma once
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
			ShaderCount,
		};
	}

	namespace EPsoID
	{
		enum
		{
			GlobalPso,
			Skybox,
			ShadowMap,
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
}