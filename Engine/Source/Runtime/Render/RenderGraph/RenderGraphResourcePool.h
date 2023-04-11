#pragma once
#include "Core.h"
#include "Render/RHI/Texture.h"
#include "Render/RHI/RenderTarget.h"
#include "Render/RHI/Buffer.h"

namespace Zero
{
	class FRGResourcePool
	{
		struct FPooledTexture
		{
			Ref<FTexture2D> Texture;
			uint64_t LastUsedFrame;
		};

		struct FPooledBuffer
		{
			Ref<FBuffer> Buffer;
			uint64_t LastUsedFrame;
		};

		struct FPoolRenderTarget
		{
			Ref<FRenderTarget2D> RenderTarget;
			uint64_t LastUsedFrame;
		};

	public:
		FRGResourcePool() = default;
		void Tick();
		FTexture2D* AllocateTexture(const FTextureDesc& Desc, char const* Name);
		FBuffer* AllocateBuffer(const FBufferDesc& Desc);
		FRenderTarget2D* AllocateRenderTarget();
		void ReleaseTexture(FTexture2D* Texture);
		void ReleaseBuffer(FBuffer* Buffer);
		void ReleaseRenderTarget(FRenderTarget2D* RenderTarget);
	private:
		uint64_t m_FrameIndex = 0;
		std::vector<std::pair<FPooledTexture, bool>> m_TexturePool;
		std::vector<std::pair<FPooledBuffer, bool>> m_BufferPool;
		std::vector<std::pair<FPoolRenderTarget, bool>> m_RenderTargetPool;
	};
}