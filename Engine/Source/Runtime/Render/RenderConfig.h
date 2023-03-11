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

	struct FSceneView
	{
		ZMath::vec3 ViewPos = ZMath::vec3(0.0f);
		ZMath::mat4 View = ZMath::identity<ZMath::mat4>();
		ZMath::mat4 Proj = ZMath::identity<ZMath::mat4>();
		ZMath::mat4 ProjectionView = ZMath::identity<ZMath::mat4>();
		float Near;
		float Far;
	};

	namespace EShaderID
	{
		enum
		{
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
			InvalidPso,
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