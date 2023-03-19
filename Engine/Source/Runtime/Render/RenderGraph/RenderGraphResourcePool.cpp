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
		auto Texture = FRenderer::GetDevice()->CreateTexture2D(TextureName, Desc);
		std::pair<FPooledTexture, bool> Pair = { FPooledTexture(Texture, false), false };
		m_TexturePool.emplace_back(Pair);
		return Texture;
	}

	Ref<FBuffer> FRGResourcePool::AllocateBuffer(const FBufferDesc& Desc)
	{
		for (auto& [PoolBuffer, bActive] : m_BufferPool)
		{ 
			if (bActive && PoolBuffer.Buffer->GetDesc() == Desc)
			{
				PoolBuffer.LastUsedFrame = m_FrameIndex;
				bActive = true;
				return PoolBuffer.Buffer;
			}
		}
		std::string BufferName = std::format("PoolBuffer_{0}", m_BufferPool.size());
		auto Buffer = FRenderer::GetDevice()->CreateBuffer(Desc);
		std::pair<FPooledBuffer, bool> Pair = { FPooledBuffer(Buffer, false), false };
		m_BufferPool.emplace_back(Pair);
		return Buffer;
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

	void FRGResourcePool::ReleaseBuffer(Ref<FBuffer> Buffer)
	{
		for (auto& [PooledBuffer, bActive] : m_BufferPool)
		{
			if (bActive && PooledBuffer.Buffer == Buffer)
			{
				bActive = false;
				return;
			}
		}
	}
}
