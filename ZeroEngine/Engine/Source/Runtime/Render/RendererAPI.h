#pragma once
#include "Core.h"
#include "Platform/DX12/DX12VertexBuffer.h"
#include "Platform/DX12/DX12Texture2D.h"

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

	class IGraphicFactroy;
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
		virtual Ref<IVertexBuffer> CreateVertexBuffer(IDevice* Device, void* data, uint32_t VertexCount, FVertexBufferLayout& Layout, IVertexBuffer::EType Type = IVertexBuffer::EType::Static) = 0;
		virtual Ref<FTexture2D> CreateTexture2D(IDevice* Device, const std::string Path) = 0;
	};

	

	class FDX12Factory : public IGraphicFactroy
	{
		virtual Ref<IVertexBuffer> CreateVertexBuffer(IDevice* Device, void* data, uint32_t VertexCount, FVertexBufferLayout& Layout, IVertexBuffer::EType Type = IVertexBuffer::EType::Static)
		{
			return CreateRef<FDX1VertexBuffer>(*((FDX12Device*)Device), data, VertexCount, Layout, Type);
		}

		virtual Ref<FTexture2D> CreateTexture2D(IDevice* Device, const std::string Path)
		{
			Ref<FImage> Image = CreateRef<FImage>(Path);
			return CreateRef<FDX12Texture2D>((FDX12Device*)Device, Image);
		}
	};
}