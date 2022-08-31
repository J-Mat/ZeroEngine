#pragma once
#include "Core.h"
#include "Platform/DX12/DX12VertexBuffer.h"
#include "Platform/DX12/DX12Texture2D.h"
#include "Platform/DX12/DX12Mesh.h"
#include "Platform/Windows/WinWindow.h"
#include "Moudule/MeshLoader.h"

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
	public:
		virtual Ref<FWinWindow> CreatePlatformWindow(const FWindowsConfig& Config) = 0;
		virtual Ref<IVertexBuffer> CreateVertexBuffer(IDevice* Device, void* data, uint32_t VertexCount, FVertexBufferLayout& Layout, IVertexBuffer::EType Type = IVertexBuffer::EType::Static) = 0;
		virtual Ref<FTexture2D> CreateTexture2D(IDevice* Device, const std::string Path) = 0;
		virtual Ref<FMesh> CreateMesh(IDevice* Device, const std::vector<FMeshData>& MeshDatas, FVertexBufferLayout Layout) = 0;
	};

	

	class FDX12Factory : public IGraphicFactroy
	{
	public:
		virtual Ref<IVertexBuffer> CreateVertexBuffer(IDevice* Device, void* data, uint32_t VertexCount, FVertexBufferLayout& Layout, IVertexBuffer::EType Type = IVertexBuffer::EType::Static)
		{
			return CreateRef<FDX1VertexBuffer>(*((FDX12Device*)Device), data, VertexCount, Layout, Type);
		}

		virtual Ref<FTexture2D> CreateTexture2D(IDevice* Device, const std::string Path)
		{
			Ref<FImage> Image = CreateRef<FImage>(Path);
			FDX12Device* DX12Device = static_cast<FDX12Device*>(Device);
			return CreateRef<FDX12Texture2D>(*DX12Device, Image);
		}
		virtual Ref<FWinWindow> CreatePlatformWindow(const FWindowsConfig& Config)
		{
			return CreateRef<FWinWindow>(Config);
		}

		virtual Ref<FMesh> CreateMesh(IDevice* Device, const std::vector<FMeshData>& MeshDatas, FVertexBufferLayout Layout);
		{
			FDX12Device* DX12Device = static_cast<FDX12Device*>(Device);
			return CreateRef<FDX12Mesh>(DX12Device, MeshDatas, Layout)
		}
	};
}