#pragma once
#include "Core.h"
#include "./RHI/GraphicFactory.h"

namespace Zero
{
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

	class FRenderer
	{
	public:
		static void SetRHI(ERHI InRHI);
		static void SetRayTracerAPI(ERayTracerAPI rayTracer);

		static inline ERHI GetRHI() { return RHI; }
		static inline ERayTracerAPI GetRayTracer() { return RayTracer; }
		
		static void InitAPI();
		
		Scope<IGraphicFactroy> GraphicFactroy;
	
	private:
		static ERHI RHI;
		static ERayTracerAPI RayTracer;
	};
	
	class IGraphicFactroy
	{
		virtual FIndexBuffer* CreateIndexBuffer(unsigned int* Indices, uint32_t Count) = 0;
	}

	class FDX12IndexBuffer;
	class FDX12Factory : public IGraphicFactroy 
	{
		virtual FIndexBuffer* CreateIndexBuffer(unsigned int* Indices, uint32_t Count) 
		{
			return new FDX12IndexBuffer(Indices, Count);
		}
	}
}