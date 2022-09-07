#pragma once
#include "Core.h"

namespace Zero
{
	class FTexture2D;
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

	class FRenderTarget
	{
	public:
		FRenderTarget();

		FRenderTarget(const FRenderTarget& copy) = default;
		FRenderTarget(FRenderTarget&& copy) = default;

		FRenderTarget& operator=(const FRenderTarget& other) = default;
		FRenderTarget& operator=(FRenderTarget&& other) = default;

		virtual void ClearBuffer() = 0;
		virtual void Bind() = 0;;
		virtual void UnBind() = 0;;
		virtual void AttachTexture(EAttachmentIndex AttachmentIndex, Ref<FTexture2D> Texture2D) = 0;

		virtual void Reset()
		{
			m_Textures = std::vector<Ref<FTexture2D>>(EAttachmentIndex::NumAttachmentPoints);
		}
		virtual Ref<FTexture2D> GetTexture(EAttachmentIndex AttachmentIndex) const { return m_Textures[size_t(AttachmentIndex)]; }
		virtual const std::vector<Ref<FTexture2D>>& GetTextures() const { return m_Textures; }
		
		virtual void Resize(uint32_t Width, uint32_t Height, uint32_t Depth) = 0;
		uint32_t GetWidth() { return m_Width; }
		uint32_t GetHeight() { return m_Height; }
		
	protected:
		std::vector<Ref<FTexture2D>> m_Textures;
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
	};
}