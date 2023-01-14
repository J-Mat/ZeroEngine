#pragma once
#include "Math/ZMath.h"

namespace Zero
{
	#define RENDERLAYER_OPAQUE  (1 << 0)
	#define	RENDERLAYER_TRANSPARENT  (1 << 2)
	#define RENDERLAYER_LIGHT  (1 << 3)
	#define	RENDERLAYER_SKYBOX  (1 << 4)
	#define	RENDERLAYER_SHADOW  (1 << 5)

	#define RENDER_STAGE_FORWARD "ForwardLit"
	#define RENDER_STAGE_SHADOWMAP "ShadowMap"
	#define RENDER_STAGE_SHADOWMAP_DEBUG "ShadowMapDebug"

	#define PSO_FORWARDLIT "Shader\\ForwardLit.hlsl"
	#define PSO_DIRECT_LIGHT "DirectLight"
	#define PSO_POINT_LIGHT "PointLight"
	#define PSO_SKYBOX "SkyBox"
	#define PSO_FRESNEL "Fresnel"
	#define PSO_SHADOWMAP "ShadowMap"
	#define PSO_NDF "NDF"
	#define PSO_GGX "GGX"
	#define IBL_BRDF_LUT "IBL_BRDF_LUT"
	
	
	#define GENERATE_MIP_SHADER "Shader\\Compute\\GenerateMips_CS.hlsl"

	enum class ERHI
	{
		DirectX12,
		OpenGL,
		RHI_NUM
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
}