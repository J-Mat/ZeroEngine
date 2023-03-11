#pragma once
#include "Core.h"
#include "Render/RenderConfig.h"

namespace Zero
{
	class FSwapChain;
	class FCommandList;
	class FPipelineStateObject;
	struct FPSODescriptor;
	class FTexture2D;
	class FTextureCubemap;
	struct FTextureDesc;
	class FShader;
	class FShaderBinderDesc;
	struct FShaderDesc;
	struct FMeshData;
	class FMesh;
	class FVertexBufferLayout;
	class IDevice 
	{
	public:
		virtual void Init() = 0;
		virtual void Resize(uint32_t Width, uint32_t Height) = 0;
		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;
		virtual void PreInitWorld() = 0;
		virtual void FlushInitCommandList() = 0;
		virtual FCommandListHandle GenerateCommanList(ERenderPassType RenderPassType = ERenderPassType::Graphics) = 0;
		virtual FCommandListHandle GetSingleThreadCommadList() = 0;
		virtual void Flush() = 0;
		virtual Ref<FSwapChain>  GetSwapChain() { return nullptr; }
		virtual void CreateSwapChain(HWND hWnd) = 0;
		virtual Scope<FPipelineStateObject> CreatePSO(const FPSODescriptor& PSODescriptor) = 0;
		virtual Ref<FTexture2D> GetOrCreateTexture2D(const std::string& Filename, bool bNeedMipMap = false) = 0;
		virtual Ref<FTexture2D> CreateTexture2D(const std::string& TextureName, const FTextureDesc& Desc) = 0;
		virtual Ref<FTextureCubemap> GetOrCreateTextureCubemap(FTextureHandle Handles[CUBEMAP_TEXTURE_CNT], std::string TextureCubemapName) = 0;
		virtual Ref<FShader> CreateShader(const FShaderBinderDesc& BinderDesc, const FShaderDesc& ShaderDesc) = 0;
		virtual Ref<FShader> CreateShader(const FShaderDesc& ShaderDesc) = 0;
		virtual Ref<FMesh> CreateMesh(const std::vector<FMeshData>& MeshDatas, FVertexBufferLayout& Layout) = 0;
		virtual Ref<FMesh> CreateMesh(float* Vertices, uint32_t VertexCount, uint32_t* Indices, uint32_t IndexCount, FVertexBufferLayout& Layout) = 0;
	};

}