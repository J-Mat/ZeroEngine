#pragma once
#include "Core.h"
#include "Platform/DX12/DX12VertexBuffer.h"
#include "Platform/DX12/DX12Texture2D.h"
#include "Platform/DX12/DX12Mesh.h"
#include "Platform/Windows/WinWindow.h"
#include "Moudule/MeshGenerator.h"
#include "Render/RHI/ShaderBinder.h"
#include "Render/RHI/Shader.h"
#include "Render/RHI/RootSignature.h"
#include "Platform/DX12/Shader/DX12Shader.h"
#include "Platform/DX12/Shader/DX12ShaderBinder.h"
#include "Platform/DX12/DX12RootSignature.h"
#include "Core/Framework/Library.h"
#include "Core/Config.h"
#include "Render/Moudule/ShaderRegister.h"

namespace Zero
{
	enum class EMeshRenderLayerType
	{
		RenderLayer_Opaque = 0,
		RenderLayer_Transparent,
		RenderLayer_Alphatest,
		RenderLayer_Background,
		RenderLayer_Shawodow,
	};

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

		static Ref<IDevice> GetDevice(uint32_t Slot = 0) 
		{
			CORE_ASSERT(Slot < Devices.size(), "Slot is out of range!")
			return Devices[Slot]; 
		}
		
		static uint32_t PushDevice(Ref<IDevice> Device) 
		{
			Devices.push_back(Device);
			return uint32_t(Devices.size() - 1);
		}
		static Ref<IDevice> RemoveDevice(uint32_t Slot) 
		{ 
			CORE_ASSERT(Slot < Devices.size(), "Slot is out of range!")
			Devices.erase(Devices.begin() + Slot); 
		}
	private:
		static ERayTracerAPI RayTracer;
		static std::vector<Ref<IDevice>> Devices;
	};
	
	class IGraphicFactroy
	{
	public:
		virtual Ref<IDevice> CreateDevice() = 0;
		virtual Ref<FWinWindow> CreatePlatformWindow(const FWindowsConfig& Config) = 0;
		virtual Ref<IVertexBuffer> CreateVertexBuffer(IDevice* Device, void* data, uint32_t VertexCount, FVertexBufferLayout& Layout, IVertexBuffer::EType Type = IVertexBuffer::EType::Static) = 0;
		virtual Ref<FMesh> CreateMesh(IDevice* Device, const std::vector<FMeshData>& MeshDatas, FVertexBufferLayout& Layout) = 0;
		virtual Ref<FMesh> CreateMesh(IDevice* Device, float* Vertices, uint32_t VertexCount, uint32_t* Indices, uint32_t IndexCount, FVertexBufferLayout& Layout) = 0;
		virtual Ref<IShaderConstantsBuffer> CreateShaderConstantBuffer(IDevice* Device, FShaderConstantsDesc& Desc) = 0;
		virtual Ref<IShaderResourcesBuffer> CreateShaderResourceBuffer(IDevice* Device, FShaderResourcesDesc& Desc, IRootSignature* RootSignature) = 0;
		virtual Ref<IShader> CreateShader(IDevice* Device, const std::string& FileName, const FShaderBinderDesc& BinderDesc, const FShaderDesc& ShaderDesc) = 0;
		virtual Ref<FTexture2D> CreateTexture2D(IDevice* Device, const std::string& FileName) = 0;
	};
	

	

	class FDX12Factory : public IGraphicFactroy
	{
	public:
		virtual Ref<IDevice> CreateDevice()
		{
			auto Device = CreateRef<FDX12Device>();
			Device->Init();
			FRenderer::PushDevice(Device);
			FShaderRegister::GetInstance().RegisterDefaultShader(Device.get());
			return Device;
		}
		virtual Ref<IVertexBuffer> CreateVertexBuffer(IDevice* Device, void* data, uint32_t VertexCount, FVertexBufferLayout& Layout, IVertexBuffer::EType Type = IVertexBuffer::EType::Static)
		{
			return CreateRef<FDX12VertexBuffer>(*((FDX12Device*)Device), data, VertexCount, Layout, Type);
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

		virtual Ref<FMesh> CreateMesh(IDevice* Device, const std::vector<FMeshData>& MeshDatas, FVertexBufferLayout& Layout)
		{
			FDX12Device* DX12Device = static_cast<FDX12Device*>(Device);
			return CreateRef<FDX12Mesh>(*DX12Device, MeshDatas, Layout);
		}

		virtual Ref<FMesh> CreateMesh(IDevice* Device, float* Vertices, uint32_t VertexCount, uint32_t* Indices, uint32_t IndexCount, FVertexBufferLayout& Layout)
		{
			FDX12Device* DX12Device = static_cast<FDX12Device*>(Device);
			return CreateRef<FDX12Mesh>(*DX12Device, Vertices, VertexCount, Indices, IndexCount, Layout);
		}

		virtual Ref<IShaderConstantsBuffer> CreateShaderConstantBuffer(IDevice* Device, FShaderConstantsDesc& Desc)
		{
			FDX12Device* DX12Device = static_cast<FDX12Device*>(Device);
			return CreateRef<FDX12ShaderConstantsBuffer>(Desc);
		}

		virtual Ref<IShaderResourcesBuffer> CreateShaderResourceBuffer(IDevice* Device, FShaderResourcesDesc& Desc, IRootSignature* RootSignature)
		{
			FDX12Device* DX12Device = static_cast<FDX12Device*>(Device);
			FDX12RootSignature* D3DRootSignature = static_cast<FDX12RootSignature*>(RootSignature);
			return CreateRef<FDX12ShaderResourcesBuffer>(*DX12Device, Desc, D3DRootSignature);
		}

		virtual Ref<IShader> CreateShader(IDevice* Device, const std::string& FileName, const FShaderBinderDesc& BinderDesc, const FShaderDesc& ShaderDesc)
		{
			FDX12Device* DX12Device = static_cast<FDX12Device*>(Device);
			Ref<IShader> Shader = Library<IShader>::Fetch(FileName);
			if (Shader == nullptr)
			{
				std::filesystem::path ShaderPath = FConfig::GetInstance().GetShaderFullPath(FileName);
				Shader = CreateRef<FDX12Shader>(*DX12Device, ShaderPath.string(), BinderDesc, ShaderDesc);
				Library<IShader>::Push(FileName, Shader);
			}
			return Shader;
		}
		virtual Ref<FTexture2D> CreateTexture2D(IDevice* Device, const std::string& FileName)
		{
			FDX12Device* DX12Device = static_cast<FDX12Device*>(Device);
			Ref<FTexture2D> Texture = Library<FTexture2D>::Fetch(FileName);
			std::filesystem::path TextureFileName = FileName;
			if (Texture == nullptr)
			{
				std::filesystem::path TexturePath = FConfig::GetInstance().GetTextureFullPath(FileName);
				Ref<FImage> Image = CreateRef<FImage>(TexturePath.string());
				Texture = CreateRef<FDX12Texture2D>(*DX12Device, Image);
				Library<FTexture2D>::Push(TextureFileName.stem().string(), Texture);
			}
			return Texture;
		}
	};
}