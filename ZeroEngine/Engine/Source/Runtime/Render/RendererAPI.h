#pragma once
#include "Core.h"
#include "Core/Framework/Layer.h"
#include "Platform/DX12/GUI/DX12GuiLayer.h"
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
		virtual FLayer* CreatGuiLayer() = 0;
		virtual Ref<FWinWindow> CreatePlatformWindow(const FWindowsConfig& Config) = 0;
		virtual Ref<IVertexBuffer> CreateVertexBuffer(void* data, uint32_t VertexCount, FVertexBufferLayout& Layout, IVertexBuffer::EType Type = IVertexBuffer::EType::Static) = 0;
		virtual Ref<FMesh> CreateMesh(const std::vector<FMeshData>& MeshDatas, FVertexBufferLayout& Layout) = 0;
		virtual Ref<FMesh> CreateMesh(float* Vertices, uint32_t VertexCount, uint32_t* Indices, uint32_t IndexCount, FVertexBufferLayout& Layout) = 0;
		virtual Ref<IShaderConstantsBuffer> CreateShaderConstantBuffer(FShaderConstantsDesc& Desc) = 0;
		virtual Ref<IShaderResourcesBuffer> CreateShaderResourceBuffer(FShaderResourcesDesc& Desc, IRootSignature* RootSignature) = 0;
		virtual Ref<IShader> CreateShader(const std::string& FileName, const FShaderBinderDesc& BinderDesc, const FShaderDesc& ShaderDesc) = 0;
		virtual Ref<FTexture2D> CreateTexture2D(const std::string& FileName) = 0;
		virtual Ref<FTexture2D> CreateTexture2D(Ref<FImage> Image) = 0;
		virtual Ref<FRenderTarget> CreateRenderTarget(FRenderTargetDesc Desc) = 0;
		virtual Ref<FTexture2D> CreateDepthStencilTexture(uint32_t Width, uint32_t Height, const std::string& Name) = 0;
	};
	

	

	class FDX12Factory : public IGraphicFactroy
	{
	public:
		virtual Ref<IDevice> CreateDevice()
		{
			auto Device = CreateRef<FDX12Device>();
			Device->Init();
			FRenderer::PushDevice(Device);
			return Device;
		}

		virtual FLayer* CreatGuiLayer()
		{
			return new FDX12GuiLayer();
		}

		virtual Ref<IVertexBuffer> CreateVertexBuffer(void* data, uint32_t VertexCount, FVertexBufferLayout& Layout, IVertexBuffer::EType Type = IVertexBuffer::EType::Static)
		{
			return CreateRef<FDX12VertexBuffer>(data, VertexCount, Layout, Type);
		}



		virtual Ref<FWinWindow> CreatePlatformWindow(const FWindowsConfig& Config)
		{
			return CreateRef<FWinWindow>(Config);
		}

		virtual Ref<FMesh> CreateMesh(const std::vector<FMeshData>& MeshDatas, FVertexBufferLayout& Layout)
		{
			
			return CreateRef<FDX12Mesh>(MeshDatas, Layout);
		}

		virtual Ref<FMesh> CreateMesh(float* Vertices, uint32_t VertexCount, uint32_t* Indices, uint32_t IndexCount, FVertexBufferLayout& Layout)
		{
			
			return CreateRef<FDX12Mesh>(Vertices, VertexCount, Indices, IndexCount, Layout);
		}

		virtual Ref<IShaderConstantsBuffer> CreateShaderConstantBuffer(FShaderConstantsDesc& Desc)
		{
			
			return CreateRef<FDX12ShaderConstantsBuffer>(Desc);
		}

		virtual Ref<IShaderResourcesBuffer> CreateShaderResourceBuffer(FShaderResourcesDesc& Desc, IRootSignature* RootSignature)
		{
			FDX12RootSignature* D3DRootSignature = static_cast<FDX12RootSignature*>(RootSignature);
			return CreateRef<FDX12ShaderResourcesBuffer>(Desc, D3DRootSignature);
		}

		virtual Ref<IShader> CreateShader(const std::string& FileName, const FShaderBinderDesc& BinderDesc, const FShaderDesc& ShaderDesc)
		{
			
			Ref<IShader> Shader = TLibrary<IShader>::Fetch(FileName);
			if (Shader == nullptr)
			{
				std::filesystem::path ShaderPath = FConfig::GetInstance().GetShaderFullPath(FileName);
				Shader = CreateRef<FDX12Shader>(ShaderPath.string(), BinderDesc, ShaderDesc);
				TLibrary<IShader>::Push(FileName, Shader);
			}
			return Shader;
		}


		virtual Ref<FTexture2D> CreateTexture2D(Ref<FImage> Image)
		{
			return CreateRef<FDX12Texture2D>(Image);
		}

		virtual Ref<FTexture2D> CreateTexture2D(const std::string& FileName)
		{
			
			std::filesystem::path TextureFileName = FileName;
			Ref<FTexture2D> Texture = TLibrary<FTexture2D>::Fetch(TextureFileName.stem().string());
			if (Texture == nullptr)
			{
				std::filesystem::path TexturePath = FConfig::GetInstance().GetTextureFullPath(FileName);
				Ref<FImage> Image = CreateRef<FImage>(TexturePath.string());
				Texture = CreateRef<FDX12Texture2D>(Image);
				std::cout << TextureFileName.stem().string() << std::endl;
				TLibrary<FTexture2D>::Push(TextureFileName.stem().string(), Texture);
			}
			return Texture;
		}
		virtual Ref<FRenderTarget> CreateRenderTarget(FRenderTargetDesc Desc)
		{
			
			return CreateRef<FDX12RenderTarget>(Desc);
		}
		virtual Ref<FTexture2D> CreateDepthStencilTexture(uint32_t Width, uint32_t Height, const std::string& Name)
		{
			
			
			auto DepthStencilDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D24_UNORM_S8_UINT, Width, Height);
			// Must be set on textures that will be used as a depth-stencil buffer.
			DepthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

			// Specify optimized clear values for the depth buffer.
			D3D12_CLEAR_VALUE OptClear = {};
			OptClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			OptClear.DepthStencil = { 1.0F, 0 };
			Ref<FDX12Texture2D> DepthStencilTexture = CreateRef<FDX12Texture2D>(DepthStencilDesc, &OptClear);
			DepthStencilTexture->SetName(Utils::String2WString(Name));
			return DepthStencilTexture;
		}
	};
}