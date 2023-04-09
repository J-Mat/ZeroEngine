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

	FTexture2D* FRGResourcePool::AllocateTexture(const FTextureDesc& Desc)
	{
		for (auto& [PoolTexture, bActive] : m_TexturePool)
		{
			if (!bActive && PoolTexture.Texture->GetDesc().IsCompatible(Desc))
			{
				PoolTexture.LastUsedFrame = m_FrameIndex;
				bActive = true;
				return PoolTexture.Texture.get();
			}
		}
		std::string TextureName = std::format("PoolTexture_{0}", m_TexturePool.size());
		Ref<FTexture2D> Texture;
		Texture.reset(FGraphic::GetDevice()->CreateTexture2D(TextureName, Desc, false));
		std::pair<FPooledTexture, bool> Pair = { FPooledTexture(Texture, false), true };
		m_TexturePool.emplace_back(Pair);
		return Texture.get();
	}

	FBuffer* FRGResourcePool::AllocateBuffer(const FBufferDesc& Desc)
	{
		for (auto& [PoolBuffer, bActive] : m_BufferPool)
		{ 
			if (!bActive && PoolBuffer.Buffer->GetDesc() == Desc)
			{
				PoolBuffer.LastUsedFrame = m_FrameIndex;
				bActive = true;
				return PoolBuffer.Buffer.get();
			}
		}
		std::string BufferName = std::format("PoolBuffer_{0}", m_BufferPool.size());
		Ref<FBuffer>Buffer;
		Buffer.reset(FGraphic::GetDevice()->CreateBuffer(Desc));
		std::pair<FPooledBuffer, bool> Pair = { FPooledBuffer(Buffer,  false), true};
		m_BufferPool.emplace_back(Pair);
		return Buffer.get();
	}

	FRenderTarget2D* FRGResourcePool::AllocateRenderTarget()
	{
		for (auto& [PoolRenderTarget, bActive] : m_RenderTargetPool)
		{ 
			if (!bActive)
			{
				PoolRenderTarget.LastUsedFrame = m_FrameIndex;
				bActive = true;
				return PoolRenderTarget.RenderTarget.get();
			}
		}
		Ref<FRenderTarget2D> RenderTarget; 
		RenderTarget.reset(FGraphic::GetDevice()->CreateRenderTarget2D());
		std::pair<FPoolRenderTarget, bool> Pair = { FPoolRenderTarget(RenderTarget, false), true};
		m_RenderTargetPool.emplace_back(Pair);
		return RenderTarget.get();
	}


	void FRGResourcePool::ReleaseTexture(FTexture2D* Texture)
	{
		for (auto& [PooledTexture, bActive] : m_TexturePool)
		{
			if (!bActive && PooledTexture.Texture.get() == Texture)
			{
				bActive = false;
				return;
			}
		}
	}

	void FRGResourcePool::ReleaseBuffer(FBuffer* Buffer)
	{
		for (auto& [PooledBuffer, bActive] : m_BufferPool)
		{
			if (bActive && PooledBuffer.Buffer.get() == Buffer)
			{
				bActive = false;
				return;
			}
		}
	}

	void FRGResourcePool::ReleaseRenderTarget(FRenderTarget2D* RenderTarget)
	{
		for (auto& [PoolRenderTarget, bActive] : m_RenderTargetPool)
		{
			if (bActive && PoolRenderTarget.RenderTarget.get() == RenderTarget)
			{
				bActive = false;
				return;
			}
		}
	}

}
