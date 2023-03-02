#pragma once
#include "Core.h"
#include "Core/Framework/Layer.h"
#include "Platform/DX12/GUI/DX12GuiLayer.h"
#include "Platform/DX12/Buffer/DX12VertexBuffer.h"
#include "Platform/DX12/DX12RenderTargetCube.h"
#include "Platform/Windows/WinWindow.h"
#include "Render/RHI/Shader/ShaderBinder.h"
#include "Render/RHI/Shader/Shader.h"
#include "Render/RHI/RootSignature.h"
#include "Platform/DX12/Shader/DX12Shader.h"
#include "Platform/DX12/Shader/DX12ComputeShader.h"
#include "Platform/DX12/Shader/DX12ShaderBinder.h"
#include "Platform/DX12/DX12RootSignature.h"
#include "Core/Framework/Library.h"
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
		virtual Ref<IShaderConstantsBuffer> CreateShaderConstantBuffer(FShaderConstantsDesc& Desc) = 0;
		virtual Ref<IShaderResourcesBuffer> CreateShaderResourceBuffer(FShaderResourcesDesc& Desc, FRootSignature* RootSignature) = 0;
		virtual Ref<FComputeShader> CreateComputeShader(const FComputeShaderDesc& ComputeShaderDesc) = 0;
		virtual Ref<FBuffer> CreateBuffer(const std::string& BufferName, const FBufferDesc& Desc) = 0;
		virtual Ref<FRenderTarget2D> CreateRenderTarget2D(const FRenderTarget2DDesc& Desc) = 0;
		virtual Ref<FRenderTargetCube> CreateRenderTargetCube(const FRenderTargetCubeDesc& Desc) = 0;
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

		virtual Ref<IShaderConstantsBuffer> CreateShaderConstantBuffer(FShaderConstantsDesc& Desc)
		{
			return CreateRef<FDX12ShaderConstantsBuffer>(Desc);
		}

		virtual Ref<IShaderResourcesBuffer> CreateShaderResourceBuffer(FShaderResourcesDesc& Desc, FRootSignature* RootSignature)
		{
			FDX12RootSignature* D3DRootSignature = static_cast<FDX12RootSignature*>(RootSignature);
			return CreateRef<FDX12ShaderResourcesBuffer>(Desc, D3DRootSignature);
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

		virtual Ref<FBuffer> CreateBuffer(const std::string& BufferName, const FBufferDesc& Desc)
		{
			Ref<FBuffer> Buffer = CreateRef<FBuffer>(BufferName, Desc);
			return Buffer;
		}

		virtual Ref<FRenderTarget2D> CreateRenderTarget2D(const FRenderTarget2DDesc& Desc)
		{
			return CreateRef<FDX12RenderTarget2D>(Desc);
		}

		virtual Ref<FRenderTargetCube> CreateRenderTargetCube(const FRenderTargetCubeDesc& Desc)
		{
			return CreateRef<FDX12RenderTargetCube>(Desc);
		}
	};

#define RHI_Factory FRenderer::GraphicFactroy
}