#pragma once
#include "Core.h"
#include "Render/RHI/Texture.h"
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

	public:
		FRGResourcePool() = default;
		void Tick();
		Ref<FTexture2D> AllocateTexture(const FTextureDesc& Desc);
		Ref<FBuffer> AllocateBuffer(const FBufferDesc& Desc);
		void ReleaseTexture(Ref<FTexture2D> Texture);
		void ReleaseBuffer(Ref<FBuffer> Buffer);
	private:
		uint64_t m_FrameIndex = 0;
		std::vector<std::pair<FPooledTexture, bool>> m_TexturePool;
		std::vector<std::pair<FPooledBuffer, bool>> m_BufferPool;
	};
}