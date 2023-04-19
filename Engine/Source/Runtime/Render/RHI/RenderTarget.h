#pragma once
#include "Core.h"
#include "Texture.h"
#include "Render/RenderConfig.h"

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

	struct FRenderTexAttachment
	{
		FTexture2D* Texture = nullptr;
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
		virtual void AttachColorTexture(uint32_t AttachmentIndex, const FRenderTexAttachment& Attachment) = 0;
		virtual void AttachDepthTexture(const FRenderTexAttachment& Attachment) = 0;

		virtual void Reset()
		{
			m_ColoTexture.clear();
			m_ColoTexture.resize(7, FRenderTexAttachment());
		}
		virtual FTexture2D* GetColorTexture(uint32_t AttachmentIndex) const { return m_ColoTexture[AttachmentIndex].Texture; }
		virtual FTexture2D* GetDepthTexture() const { return m_DepthTexture.Texture; }
		
		virtual void Resize(uint32_t Width, uint32_t Height, uint32_t Depth = 1) = 0;
		uint32_t GetWidth() { return m_Width; }
		uint32_t GetHeight() { return m_Height; }
		
	protected:
		std::vector<FRenderTexAttachment> m_ColoTexture;
		FRenderTexAttachment m_DepthTexture;
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
	};


	struct FRenderTargetCubeDesc
	{
		std::string RenderTargetName = "default";
		uint32_t Size = 900;
		bool bRenderDepth = false;
		EResourceFormat TextureFormat = EResourceFormat::R8G8B8A8_UNORM;
	};

	class FRenderTargetCube
	{
	public:	
		FRenderTargetCube(const FRenderTargetCubeDesc& Desc);
		virtual void SetRenderTarget(uint32_t Index, FCommandListHandle CommandListHandle) = 0;
		virtual void Bind(FCommandListHandle CommandListHandle) = 0;
		virtual void UnBind(FCommandListHandle CommandListHandle) = 0;
		const FSceneView& GetSceneView(uint32_t Index) { return SceneViews[Index]; }
		Ref<FTextureCube> GetColorCubemap() {return m_TextureColorCubemap; }
		Ref<FTextureCube> GetDepthCubemap() { return m_TextureDepthCubemap; }
	protected:
		Ref<FTextureCube> m_TextureColorCubemap;
		Ref<FTextureCube> m_TextureDepthCubemap;
		uint32_t m_Size = 0;
		bool m_bRenderDepth;
		FSceneView SceneViews[6];
	};
}