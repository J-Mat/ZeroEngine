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




		virtual void AttachTexture(EAttachmentIndex AttachmentIndex, Ref<FTexture2D> Texture2D) = 0;
		// virtual void AttachTexture(EAttachmentIndex AttachmentIndex, Ref<FDX12Texture2D> Texture2D);



		virtual void Reset() = 0;
		virtual Ref<FTexture2D> GetTexture(EAttachmentIndex AttachmentIndex) = 0;
		
		virtual void Resize(uint32_t Width, uint32_t Height);
		uint32_t GetWidth() { return m_Width; }
		uint32_t GetHeight() { return m_Height; }
		
	protected:
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
	};
}