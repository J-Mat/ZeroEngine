#include "RenderGraphResourcePool.h"
#include "Render/RendererAPI.h"


namespace Zero
{
	void FRGResourcePool::Tick()
	{
		for (size_t i = 0; i < m_Texture2DPool.size();)
		{
			FPooledTexture2D& Resource = m_Texture2DPool[i].first;
			bool bActive = m_Texture2DPool[i].second;
			if (!bActive && Resource.LastUsedFrame + 4 < m_FrameIndex)
			{
				std::swap(m_Texture2DPool[i], m_Texture2DPool.back());
				m_Texture2DPool.pop_back();
			}
			else
			{
				++i;
			}
		}
		++m_FrameIndex;
	}

	FTexture2D* FRGResourcePool::AllocateTexture2D(const FTextureDesc& Desc, char const* Name)
	{
		std::string TextureName = std::format("PoolTexture2D_{0}_{1}", m_Texture2DPool.size(), Name);
		for (auto& [PoolTexture, bActive] : m_Texture2DPool)
		{
			if (!bActive && PoolTexture.Texture->GetDesc().IsCompatible(Desc))
			{
				PoolTexture.Texture->SetName(TextureName);
				PoolTexture.LastUsedFrame = m_FrameIndex;
				bActive = true;
				return PoolTexture.Texture.get();
			}
		}
		Scope<FTexture2D> Texture;
		Texture.reset(FGraphic::GetDevice()->CreateTexture2D(TextureName, Desc, false));
		std::pair<FPooledTexture2D, bool> Pair = { FPooledTexture2D(std::move(Texture), m_FrameIndex), true };
		m_Texture2DPool.emplace_back(std::move(Pair));
		return m_Texture2DPool.back().first.Texture.get();
	}

	FTextureCube* FRGResourcePool::AllocateTextureCube(const FTextureDesc& Desc, char const* Name)
	{
		std::string TextureName = std::format("PoolTextureCube_{0}_{1}", m_TextureCubePool.size(), Name);
		for (auto& [PoolTexture, bActive] : m_TextureCubePool)
		{
			if (!bActive && PoolTexture.Texture->GetDesc().IsCompatible(Desc))
			{
				PoolTexture.Texture->SetName(TextureName);
				PoolTexture.LastUsedFrame = m_FrameIndex;
				bActive = true;
				return PoolTexture.Texture.get();
			}
		}
		Scope<FTextureCube> Texture;
		Texture.reset(FGraphic::GetDevice()->CreateTextureCube(TextureName, Desc, false));
		std::pair<FPooledTextureCube, bool> Pair = { FPooledTextureCube(std::move(Texture), m_FrameIndex), true };
		m_TextureCubePool.emplace_back(std::move(Pair));
		return m_TextureCubePool.back().first.Texture.get();
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


	FRenderTarget2D* FRGResourcePool::AllocateRenderTarget2D()
	{
		for (auto& [PoolRenderTarget, bActive] : m_RenderTarget2DPool)
		{ 
			if (!bActive)
			{
				PoolRenderTarget.LastUsedFrame = m_FrameIndex;
				bActive = true;
				PoolRenderTarget.RenderTarget->Reset();
				return PoolRenderTarget.RenderTarget.get();
			}
		}
		Ref<FRenderTarget2D> RenderTarget; 
		RenderTarget.reset(FGraphic::GetDevice()->CreateRenderTarget2D());
		std::pair<FPoolRenderTarget2D, bool> Pair = { FPoolRenderTarget2D(RenderTarget, false), true};
		m_RenderTarget2DPool.emplace_back(Pair);
		return RenderTarget.get();
	}


	FRenderTargetCube* FRGResourcePool::AllocateRenderTargetCube()
	{
		for (auto& [PoolRenderTarget, bActive] : m_RenderTargetCubePool)
		{ 
			if (!bActive)
			{
				PoolRenderTarget.LastUsedFrame = m_FrameIndex;
				bActive = true;
				//PoolRenderTarget.RenderTarget->Reset();
				return PoolRenderTarget.RenderTarget.get();
			}
		}
		Ref<FRenderTargetCube> RenderTargetCube; 
		RenderTargetCube.reset(FGraphic::GetDevice()->CreateRenderTargetCube());
		std::pair<FPoolRenderTargetCube, bool> Pair = { FPoolRenderTargetCube(RenderTargetCube, false), true};
		m_RenderTargetCubePool.emplace_back(Pair);
		return RenderTargetCube.get();
	}

	void FRGResourcePool::ReleaseTexture2D(FTexture2D* Texture)
	{
		for (auto& [PooledTexture2D, bActive] : m_Texture2DPool)
		{
			if (bActive && PooledTexture2D.Texture.get() == Texture)
			{
				bActive = false;
				return;
			}
		}
	}

	void FRGResourcePool::ReleaseTextureCube(FTextureCube* Texture)
	{
		for (auto& [PooledTextureCube, bActive] : m_TextureCubePool)
		{
			if (bActive && PooledTextureCube.Texture.get() == Texture)
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

	void FRGResourcePool::ReleaseRenderTarget2D(FRenderTarget2D* RenderTarget)
	{
		for (auto& [PoolRenderTarget, bActive] : m_RenderTarget2DPool)
		{
			if (bActive && PoolRenderTarget.RenderTarget.get() == RenderTarget)
			{
				bActive = false;
				return;
			}
		}
	}

	void FRGResourcePool::ReleaseRenderTargetCube(FRenderTargetCube* RenderTarget)
	{
		for (auto& [PoolRenderTarget, bActive] : m_RenderTargetCubePool)
		{
			if (bActive && PoolRenderTarget.RenderTarget.get() == RenderTarget)
			{
				bActive = false;
				return;
			}
		}
	}

}
