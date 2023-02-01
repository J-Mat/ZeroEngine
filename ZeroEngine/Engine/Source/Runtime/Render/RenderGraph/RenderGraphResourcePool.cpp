#include "RenderGraphResourcePool.h"
#include "Render/RendererAPI.h"


namespace Zero
{
	void FRGResourcePool::Tick()
	{
		for (size_t i = 0; i < m_TexturePool.size();)
		{
			FPooledTexture& Resource = m_TexturePool[i].first;
			bool bActive = m_TexturePool[i].second;
			if (bActive && Resource.LastUsedFrame + 4 < m_FrameIndex)
			{
				std::swap(m_TexturePool[i], m_TexturePool.back());
				m_TexturePool.pop_back();
			}
			else
			{
				++i;
			}
		}
		++m_FrameIndex;
	}
	Ref<FTexture2D> FRGResourcePool::AllocateTexture(FTextureDesc const& Desc)
	{
		for (auto& [PoolTexture, bActive] : m_TexturePool)
		{
			if (bActive && PoolTexture.Texture->GetDesc().IsCompatible(Desc))
			{
				PoolTexture.LastUsedFrame = m_FrameIndex;
				bActive = true;
				return PoolTexture.Texture;
			}
		}
		std::string TextureName = std::format("PoolTexture_{0}", m_TexturePool.size());
		auto Texture = FRenderer::GraphicFactroy->CreateTexture2D(TextureName, Desc);
		m_TexturePool.emplace_back(FPooledTexture{ Texture, false });
		return Texture;
	}

	void FRGResourcePool::ReleaseTexture(Ref<FTexture2D> Texture)
	{
		for (auto& [PooledTexture, bActive] : m_TexturePool)
		{
			if (bActive && PooledTexture.Texture == Texture)
			{
				bActive = false;
				return;
			}
		}
	}
}
