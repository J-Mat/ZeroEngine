#pragma once
#include "Core.h"
#include "Texture.h"
#include "Render/RenderConfig.h"
#include "Render/Moudule/SceneCapture.h"

namespace Zero
{
	struct FRenderTarget2DDesc
	{
		std::string RenderTargetName = "default";
		uint32_t Width = 900;
		uint32_t Height = 600;
		FFrameBufferTexturesFormats ColorFormat;
		bool bNeedDepth = true;
		EResourceFormat DepthFormat = EResourceFormat::D24_UNORM_S8_UINT;
		std::strong_ordering operator<=>(FRenderTarget2DDesc const& Other) const = default;
	};

	struct FRenderTex2DAttachment
	{
		FTexture2D* Texture = nullptr;
		uint32_t ViewID = 0;
		std::optional<FTextureClearValue> ClearValue = std::nullopt;
		bool bClearColor = true;
	};

	struct FRenderTexCubeAttachment
	{
		FTextureCube* Texture = nullptr;
		uint32_t ViewID = 0;
		std::optional<FTextureClearValue> ClearValue = std::nullopt;
		bool bClearColor = true;
	};

	class FRenderTarget2D
	{
	public:
		FRenderTarget2D();

		FRenderTarget2D(const FRenderTarget2D& copy) = default;
		FRenderTarget2D(FRenderTarget2D&& copy) = default;

		FRenderTarget2D& operator=(const FRenderTarget2D& other) = default;
		FRenderTarget2D& operator=(FRenderTarget2D&& other) = default;

		virtual void ClearBuffer(FCommandListHandle CommandListHandle) = 0;
		virtual void Bind(FCommandListHandle CommanListHandle) = 0;
		virtual void UnBind(FCommandListHandle CommandListHandle) = 0;
		virtual void UnBindDepth(FCommandListHandle CommandListHandle) = 0;
		virtual void AttachColorTexture(uint32_t AttachmentIndex, const FRenderTex2DAttachment& Attachment) = 0;
		virtual void AttachDepthTexture(const FRenderTex2DAttachment& Attachment) = 0;

		virtual void Reset();
		virtual FTexture2D* GetColorTexture(uint32_t AttachmentIndex) const { return m_ColoTexture[AttachmentIndex].Texture; }
		virtual FTexture2D* GetDepthTexture() const { return m_DepthTexture.Texture; }
		
		virtual void Resize(uint32_t Width, uint32_t Height, uint32_t Depth = 1) = 0;
		uint32_t GetWidth() { return m_Width; }
		uint32_t GetHeight() { return m_Height; }
		
	protected:
		std::vector<FRenderTex2DAttachment> m_ColoTexture;
		FRenderTex2DAttachment m_DepthTexture;
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
	};


	struct FRenderTargetCubeDesc
	{
		std::string RenderTargetName = "default";
		uint32_t Size = 900;
		uint16_t MipLevels = 1;
		bool bRenderDepth = false;
		EResourceFormat TextureFormat = EResourceFormat::R8G8B8A8_UNORM;
	};

	class FRenderTargetCube
	{
	public:	
		FRenderTargetCube();
		FRenderTargetCube(const FRenderTargetCubeDesc& Desc);
		void InitParams();
		virtual void Resize(uint32_t Width, uint32_t Height) = 0;
		virtual void Bind(FCommandListHandle CommandListHandle) = 0;
		virtual void SetRenderTarget(FCommandListHandle CommandListHandle, uint32_t FaceIndex, uint32_t SubResource = -1) = 0;
		virtual void UnBind(FCommandListHandle CommandListHandle, uint32_t FaceIndex, uint32_t SubResource = -1) = 0;
		const FSceneView& GetSceneView(uint32_t Index) { return m_SceneCaptureCube.GetSceneView(Index); }
		Ref<IShaderConstantsBuffer> GetCamera(uint32_t Index) {	return m_SceneCaptureCube.GetCamera(Index);}
		void AttachColorTexture(const FRenderTexCubeAttachment& Attachment) { m_TextureColorCubemap = Attachment; }
		void AttachDepthTexture(const FRenderTexCubeAttachment& Attachment) { m_TextureDepthCubemap = Attachment; }
		FTextureCube* GetColorTexCube() {return m_TextureColorCubemap.Texture;}
		FTextureCube* GetDepthTexCube() {return m_TextureDepthCubemap.Texture;}
	protected:
		FSceneCaptureCube m_SceneCaptureCube;
		FRenderTargetCubeDesc m_RenderTargetCubeDesc;
		FRenderTexCubeAttachment m_TextureColorCubemap;
		FRenderTexCubeAttachment m_TextureDepthCubemap;
	};
}