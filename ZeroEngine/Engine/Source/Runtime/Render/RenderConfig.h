#pragma once

namespace Zero
{
	#define RENDERLAYER_OPAQUE  (1 << 0)
	#define	RENDERLAYER_TRANSPARENT  (1 << 1)
	#define RENDERLAYER_ALPHATEST  (1 << 2)
	#define	RENDERLAYER_SKYBOX  (1 << 3)
	#define	RENDERLAYER_SHAWODOW  (1 << 4)

	#define RENDER_STAGE_SKYBOX "SkyBox"
	#define RENDER_STAGE_FORWARD "ForwardLit"

	#define PSO_FORWARDLIT "ForwardLit"
	#define PSO_LIGHT "Light"
	#define PSO_SKYBOX "SkyBox"
	#define PSO_FRESNEL "Fresnel"
	#define PSO_NDF "NDF"
	#define PSO_GGX "GGX"



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
		CULL_MODE_FRONT,
		CULL_MODE_BACK,
	};
}