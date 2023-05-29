#pragma once
#include "Core.h"
#include "Render/RenderConfig.h"

namespace Zero
{
	class FSwapChain;
	class FCommandList;
	class FGraphicPipelineStateObject;
	class FComputePipelineStateObject;
	struct FGraphicPSODescriptor;
	struct FComputePSODescriptor;
	class FTexture2D;
	class FTextureCube;
	struct FTextureDesc;
	class FShader;
	class FShaderBinderDesc;
	struct FShaderDesc;
	struct FMeshData;
	class FMesh;
	class FVertexBufferLayout;
	class FBuffer;
	struct FBufferDesc;
	class FRenderTarget2D;
	struct FRenderTarget2DDesc;
	struct FRenderTargetCubeDesc;
	class FRenderTargetCube;
	class FResourceBarrierBatch;
	class FShader;
	class IDevice 
	{
	public:
		static constexpr UINT BACKBUFFER_COUNT = 3;
		virtual void Init() = 0;
		virtual void Resize(uint32_t Width, uint32_t Height) = 0;
		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;
		virtual void PreInitWorld() = 0;
		virtual void FlushInitCommandList() = 0;
		virtual FCommandListHandle GenerateCommanList(ERenderPassType RenderPassType = ERenderPassType::Graphics) = 0;
		virtual FCommandListHandle GetSingleThreadCommadList(ERenderPassType RenderPassType = ERenderPassType::Graphics) = 0;
		virtual void Flush() = 0;
		virtual Ref<FSwapChain>  GetSwapChain() { return nullptr; }
		virtual void CreateSwapChain(HWND hWnd) = 0;
		virtual Ref<FCommandList> GetRHICommandList(FCommandListHandle Handle, ERenderPassType RenderPassType = ERenderPassType::Graphics) = 0;
		virtual Ref<FGraphicPipelineStateObject> CreateGraphicPSO(const FGraphicPSODescriptor& PSODescriptor) = 0;
		virtual Ref<FComputePipelineStateObject> CreateComputePSO(const FComputePSODescriptor& PSODescriptor) = 0;
		virtual Ref<FTexture2D> GetOrCreateTexture2D(const std::string& Filename, bool bNeedMipMap = false) = 0;
		virtual FTexture2D* CreateTexture2D(const std::string& TextureName, const FTextureDesc& Desc, bool bCreateTextureView) = 0;
		virtual FTextureCube* CreateTextureCube(const std::string& TextureName, const FTextureDesc& Desc, bool bCreateTextureView) = 0;
		virtual Ref<FTextureCube> GetOrCreateTextureCubemap(FTextureHandle Handles[CUBEMAP_TEXTURE_CNT], std::string TextureCubemapName) = 0;
		virtual Ref<FShader> CreateGraphicShader(const FShaderDesc& ShaderDesc) = 0;
		virtual Ref<FShader> CreateComputeShader(const FShaderDesc& ShaderDesc) = 0;
		virtual Ref<FMesh> CreateMesh(const std::vector<FMeshData>& MeshDatas, FVertexBufferLayout& Layout) = 0;
		virtual Ref<FMesh> CreateMesh(float* Vertices, uint32_t VertexCount, uint32_t* Indices, uint32_t IndexCount, FVertexBufferLayout& Layout) = 0;
		virtual FBuffer* CreateBuffer(const FBufferDesc& Desc) = 0;
		virtual void BindVertexBuffer(FCommandListHandle Handle, FBuffer* VertexBuffer) = 0;
		virtual void BindIndexBuffer(FCommandListHandle Handle, FBuffer* IndexBuffer) = 0;
		virtual Ref<FRenderTarget2D> CreateRenderTarget2D(const FRenderTarget2DDesc& Desc) = 0;
		virtual FRenderTarget2D* CreateRenderTarget2D() = 0;
		virtual FRenderTargetCube* CreateRenderTargetCube() = 0;
		virtual Ref<FRenderTargetCube> CreateRenderTargetCube(const FRenderTargetCubeDesc& Desc) = 0;
	};
}