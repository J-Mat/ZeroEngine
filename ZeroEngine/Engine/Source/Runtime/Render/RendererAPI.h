#pragma once
#include "Core.h"
#include "RHI/VertexBuffer.h"

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
		static void SetRayTracerAPI(ERayTracerAPI rayTracer) {}

		static inline ERHI GetRHI() { return RHI; }
		static inline ERayTracerAPI GetRayTracer() { return RayTracer; }
		
		static void InitAPI();
		
		static Scope<IGraphicFactroy> GraphicFactroy;
	
		static ERHI RHI;
	private:
		static ERayTracerAPI RayTracer;
	};
	
	class IGraphicFactroy
	{
		virtual Ref<IVertexBuffer> CreateVertexBuffer(void* data, uint32_t VertexCount, FVertexBufferLayout& Layout, IVertexBuffer::EType Type = IVertexBuffer::EType::Static) = 0;
	};

	class FDX12IndexBuffer;
	class FDX12VertexBuffer;
	class FDX12Factory : public IGraphicFactroy
	{
		virtual Ref<IVertexBuffer> CreateVertexBuffer(void* data, uint32_t VertexCount, FVertexBufferLayout& Layout, IVertexBuffer::EType Type = IVertexBuffer::EType::Static)
		{
			return FDX12VertexBuffer(data, VertexCount, Layout, Type);
		}
	};
}