#pragma once

namespace Zero
{
	enum class ERHI
	{
		DirectX12,
		OpenGL,
	};

	enum class ERayTracerAPI
	{
		CpuSoftware,
		DXR,
	};

	class FRenderer
	{
	public:
		static inline void SetRHI(ERHI InRHI) { RHI = InRHI; }
		static void SetRayTracerAPI(ERayTracerAPI rayTracer);

		static inline ERHI GetRHI() { return RHI; }
		static inline ERayTracerAPI GetRayTracer() { return RayTracer; }

	private:
		static ERHI RHI;
		static ERayTracerAPI RayTracer;
	};

}