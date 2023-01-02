#pragma once
#include "Core.h"
#include "Texture.h"
#include "Render/RenderConfig.h"

namespace Zero
{
	enum EAttachmentIndex
	{
		Color0,
		Color1,
		Color2,
		Color3,
		Color4,
		Color5,
		Color6,
		Color7,
		DepthStencil,
		NumAttachmentPoints
	};

	struct FRenderTarget2DDesc
	{
		std::string RenderTargetName = "default";
		uint32_t Width = 900;
		uint32_t Height = 600;
		FFrameBufferTexturesFormats Format;
	};

	class FRenderTarget2D
	{
	public:
		FRenderTarget2D();

		FRenderTarget2D(const FRenderTarget2D& copy) = default;
		FRenderTarget2D(FRenderTarget2D&& copy) = default;

		FRenderTarget2D& operator=(const FRenderTarget2D& other) = default;
		FRenderTarget2D& operator=(FRenderTarget2D&& other) = default;

		virtual void ClearBuffer() = 0;
		virtual void Bind(bool bClearBuffer = true) = 0;
		virtual void UnBind() = 0;
		virtual void UnBindDepth() = 0;
		virtual void AttachTexture(EAttachmentIndex AttachmentIndex, Ref<FTexture2D> Texture2D) = 0;

		virtual void Reset()
		{
			m_Textures = std::vector<Ref<FTexture2D>>(EAttachmentIndex::NumAttachmentPoints);
		}
		virtual Ref<FTexture2D> GetTexture(EAttachmentIndex AttachmentIndex) const { return m_Textures[size_t(AttachmentIndex)]; }
		virtual const std::vector<Ref<FTexture2D>>& GetTextures() const { return m_Textures; }
		
		virtual void Resize(uint32_t Width, uint32_t Height, uint32_t Depth = 1) = 0;
		uint32_t GetWidth() { return m_Width; }
		uint32_t GetHeight() { return m_Height; }
		
	protected:
		std::vector<Ref<FTexture2D>> m_Textures;
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
	};


	struct FRenderTargetCubeDesc
	{
		std::string RenderTargetName = "default";
		uint32_t Size = 900;
		bool bRenderDepth = false;
		ETextureFormat TextureFormat = ETextureFormat::R8G8B8A8;
	};

	class FRenderTargetCube
	{
	public:	
		FRenderTargetCube(const FRenderTargetCubeDesc& Desc);
		virtual void SetRenderTarget(uint32_t Index) = 0;
		virtual void Bind() = 0;
		virtual void UnBind() = 0;
		const FSceneView& GetSceneView(uint32_t Index) { return SceneViews[Index]; }
		Ref<FTextureCubemap> GetColorCubemap() {return m_TextureColorCubemap; }
		Ref<FTextureCubemap> GetDepthCubemap() { return m_TextureDepthCubemap; }
	protected:
		Ref<FTextureCubemap> m_TextureColorCubemap;
		Ref<FTextureCubemap> m_TextureDepthCubemap;
		uint32_t m_Size = 0;
		bool m_bRenderDepth;
		FSceneView SceneViews[6];
	};
}