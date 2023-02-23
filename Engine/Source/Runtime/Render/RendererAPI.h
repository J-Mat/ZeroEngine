#pragma once
#include "Core.h"
#include "Core/Framework/Layer.h"
#include "Platform/DX12/GUI/DX12GuiLayer.h"
#include "Platform/DX12/Buffer/DX12VertexBuffer.h"
#include "Platform/DX12/DX12Texture2D.h"
#include "Platform/DX12/DX12TextureCubemap.h"
#include "Platform/DX12/DX12Mesh.h"
#include "Platform/DX12/DX12RenderTargetCube.h"
#include "Platform/Windows/WinWindow.h"
#include "Moudule/MeshGenerator.h"
#include "Render/RHI/ShaderBinder.h"
#include "Render/RHI/Shader.h"
#include "Render/RHI/RootSignature.h"
#include "Platform/DX12/Shader/DX12Shader.h"
#include "Platform/DX12/Shader/DX12ComputeShader.h"
#include "Platform/DX12/Shader/DX12ShaderBinder.h"
#include "Platform/DX12/DX12RootSignature.h"
#include "Platform/DX12/PSO/DX12PipelineStateObject.h"
#include "Core/Framework/Library.h"
#include "ZConfig.h"
#include "Render/Moudule/PSORegister.h"
#include "RenderConfig.h"

namespace Zero
{
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
		virtual Ref<FMesh> CreateMesh(const std::vector<FMeshData>& MeshDatas, FVertexBufferLayout& Layout) = 0;
		virtual Ref<FMesh> CreateMesh(float* Vertices, uint32_t VertexCount, uint32_t* Indices, uint32_t IndexCount, FVertexBufferLayout& Layout) = 0;
		virtual Ref<IShaderConstantsBuffer> CreateShaderConstantBuffer(FShaderConstantsDesc& Desc) = 0;
		virtual Ref<IShaderResourcesBuffer> CreateShaderResourceBuffer(FShaderResourcesDesc& Desc, FRootSignature* RootSignature) = 0;
		virtual Ref<FShader> CreateShader(const FShaderBinderDesc& BinderDesc, const FShaderDesc& ShaderDesc) = 0;
		virtual Ref<FShader> CreateShader(const FShaderDesc& ShaderDesc) = 0;
		virtual Ref<FComputeShader> CreateComputeShader(const FComputeShaderDesc& ComputeShaderDesc) = 0;
		virtual Ref<FTexture2D> GetOrCreateTexture2D(const std::string& Filename, bool bNeedMipMap = false) = 0;
		virtual Ref<FTexture2D> CreateTexture2D(Ref<FImage> Image, std::string ImageName, bool bNeedMipMap = false) = 0;
		virtual Ref<FTexture2D> CreateTexture2D(const std::string& TextureName, const FTextureDesc& Desc) = 0;
		virtual Ref<FBuffer> CreateBuffer(const std::string& BufferName, const FBufferDesc& Desc) = 0;
		virtual Ref<FTextureCubemap> GetOrCreateTextureCubemap(FTextureHandle Handles[CUBEMAP_TEXTURE_CNT], std::string TextureCubemapName) = 0;
		virtual Ref<FRenderTarget2D> CreateRenderTarget2D(const FRenderTarget2DDesc& Desc) = 0;
		virtual Ref<FRenderTargetCube> CreateRenderTargetCube(const FRenderTargetCubeDesc& Desc) = 0;
		virtual Ref<FTexture2D> CreateDepthStencilTexture(uint32_t Width, uint32_t Height, const std::string& Name) = 0;
		virtual Ref<FPipelineStateObject> CreatePSO(const std::string& PSOHandle, const FPSODescriptor& PSODescriptor) = 0;
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

		virtual Ref<IShaderResourcesBuffer> CreateShaderResourceBuffer(FShaderResourcesDesc& Desc, FRootSignature* RootSignature)
		{
			FDX12RootSignature* D3DRootSignature = static_cast<FDX12RootSignature*>(RootSignature);
			return CreateRef<FDX12ShaderResourcesBuffer>(Desc, D3DRootSignature);
		}

		virtual Ref<FShader> CreateShader(const FShaderBinderDesc& BinderDesc, const FShaderDesc& ShaderDesc)
		{
			Ref<FShader> Shader = TLibrary<FShader>::Fetch(ShaderDesc.ShaderName);
			if (Shader == nullptr)
			{
				Shader = CreateRef<FDX12Shader>(BinderDesc, ShaderDesc);
				TLibrary<FShader>::Push(ShaderDesc.ShaderName, Shader);
			}
			return Shader;
		}

		virtual Ref<FShader> CreateShader(const FShaderDesc& ShaderDesc)
		{
			Ref<FShader> Shader = TLibrary<FShader>::Fetch(ShaderDesc.ShaderName);
			if (Shader == nullptr)
			{
				Shader = CreateRef<FDX12Shader>(ShaderDesc);
				TLibrary<FShader>::Push(ShaderDesc.ShaderName, Shader);
			}
			return Shader;
		}

		virtual Ref<FComputeShader> CreateComputeShader(const FComputeShaderDesc& ComputeShaderDesc)
		{
			Ref<FComputeShader> Shader = TLibrary<FComputeShader>::Fetch(ComputeShaderDesc.ShaderName);
			if (Shader == nullptr)
			{
				Shader = CreateRef<FDX12ComputeShader>(ComputeShaderDesc);
				TLibrary<FComputeShader>::Push(ComputeShaderDesc.ShaderName, Shader);
			}
			return Shader;
		}

		virtual Ref<FTexture2D> CreateTexture2D(Ref<FImage> Image, std::string ImageName, bool bNeedMipMap = false)
		{
			Ref<FTexture2D> Texture = CreateRef<FDX12Texture2D>(ImageName, Image, bNeedMipMap);
#if	EDITOR_MODE
			Texture->RegistGuiShaderResource();
#endif
			TLibrary<FTexture2D>::Push(ImageName, Texture);
			return Texture;
		}

		virtual Ref<FBuffer> CreateBuffer(const std::string& BufferName, const FBufferDesc& Desc)
		{
			Ref<FBuffer> Buffer = CreateRef<FBuffer>(BufferName, Desc);
			return Buffer;
		}

		virtual Ref<FTexture2D> CreateTexture2D(const std::string& TextureName, const FTextureDesc& Desc)
		{
			Ref<FTexture2D> Texture = CreateRef<FDX12Texture2D>(TextureName, Desc);
			return Texture;
		}

		virtual Ref<FTexture2D> GetOrCreateTexture2D(const std::string& Filename, bool bNeedMipMap = false)
		{
			std::filesystem::path TextureFileName = Filename;
			Ref<FTexture2D> Texture{};
			std::filesystem::path TexturePath = ZConfig::GetAssestsFullPath(Filename);
			if (std::filesystem::exists(TexturePath))
			{
				Ref<FImage> Image = CreateRef<FImage>(TexturePath.string());
				Texture = CreateRef<FDX12Texture2D>(Filename, Image, bNeedMipMap);
#if	EDITOR_MODE
				Texture->RegistGuiShaderResource();
#endif
			}
			return Texture;
		}

		virtual Ref<FTextureCubemap> GetOrCreateTextureCubemap(FTextureHandle Handles[CUBEMAP_TEXTURE_CNT], std::string TextureCubemapName) 
		{
			return nullptr;
			/*
			auto CreateTextureCubemap = [&]() -> Ref<FTextureCubemap>
			{
				Ref<FImage> ImageData[CUBEMAP_TEXTURE_CNT];
				for (int i = 0; i < CUBEMAP_TEXTURE_CNT; ++i)
				{
					std::filesystem::path TexturePath = ZConfig::GetAssestsFullPath(Handles[i]);
					ImageData[i] = CreateRef<FImage>(TexturePath.string());
				}
				return CreateRef<FDX12TextureCubemap>(ImageData);
			};
			*/

			std::filesystem::path TextureFileName = TextureCubemapName;
			Ref<FTextureCubemap> TextureCubemap = TLibrary<FTextureCubemap>::Fetch(TextureCubemapName);
			if (TextureCubemap == nullptr)
			{
				Ref<FImage> ImageData[CUBEMAP_TEXTURE_CNT];
				for (int i = 0; i < CUBEMAP_TEXTURE_CNT; ++i)
				{
					Ref<FTexture2D> Texture2D = GetOrCreateTexture2D(Handles[i].TextureName);
					ImageData[i] = Texture2D->GetImage();
				}
				TextureCubemap = CreateRef<FDX12TextureCubemap>(TextureCubemapName, ImageData);
			}
			else
			{
				TLibrary<FTextureCubemap>::Remove(TextureCubemapName);
				TextureCubemap.reset();
				Ref<FImage> ImageData[CUBEMAP_TEXTURE_CNT]	;
				for (int i = 0; i < CUBEMAP_TEXTURE_CNT; ++i)
				{
					Ref<FTexture2D> Texture2D = GetOrCreateTexture2D(Handles[i].TextureName);
					ImageData[i] = Texture2D->GetImage();
				}
				TextureCubemap = CreateRef<FDX12TextureCubemap>(TextureCubemapName, ImageData);
			}
			TLibrary<FTextureCubemap>::Push(TextureCubemapName, TextureCubemap);
			return TextureCubemap;
		}

		virtual Ref<FRenderTarget2D> CreateRenderTarget2D(const FRenderTarget2DDesc& Desc)
		{
			return CreateRef<FDX12RenderTarget2D>(Desc);
		}

		virtual Ref<FRenderTargetCube> CreateRenderTargetCube(const FRenderTargetCubeDesc& Desc)
		{
			return CreateRef<FDX12RenderTargetCube>(Desc);
		}

		virtual Ref<FTexture2D> CreateDepthStencilTexture(uint32_t Width, uint32_t Height, const std::string& Name)
		{
			auto DepthStencilDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D24_UNORM_S8_UINT, Width, Height);
			// Must be set on textures that will be used as a Depth-Stencil buffer.
			DepthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

			// Specify optimized clear values for the Depth buffer.
			D3D12_CLEAR_VALUE OptClear = {};
			OptClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			OptClear.DepthStencil = { 1.0F, 0 };
			FDX12TextureSettings Settings
			{
				.Desc = DepthStencilDesc
			};
			Ref<FDX12Texture2D> DepthStencilTexture = CreateRef<FDX12Texture2D>(Name, Settings, &OptClear);
			return DepthStencilTexture;
		}

		virtual Ref<FPipelineStateObject> CreatePSO(const std::string& PSOHandle, const FPSODescriptor& PSODescriptor)
		{
			auto PSO = CreateRef<FDX12PipelineStateObject>(PSODescriptor);
			TLibrary<FPipelineStateObject>::Push(PSOHandle, PSO);
			return PSO;
		}
	};

#define RHI_Factory FRenderer::GraphicFactroy
}