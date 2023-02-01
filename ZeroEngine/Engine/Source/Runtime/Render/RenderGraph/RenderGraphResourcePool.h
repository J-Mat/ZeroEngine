#pragma once
#include "Core.h"
#include "Render/RHI/Texture.h"

namespace Zero
{
	class FRGResourcePool
	{
		struct FPooledTexture
		{
			Ref<FTexture2D> Texture;
			uint64_t LastUsedFrame;
		};
	public:
		FRGResourcePool() = default;
		void Tick();
		Ref<FTexture2D> AllocateTexture(FTextureDesc const& Desc);
		void ReleaseTexture(Ref<FTexture2D> Texture);
	private:
		uint64_t m_FrameIndex = 0;
		std::vector<std::pair<FPooledTexture, bool>> m_TexturePool;
	};
}