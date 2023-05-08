#pragma once
#include "Core.h"
#include "Render/RHI/Texture.h"
#include "Render/RHI/RenderTarget.h"
#include "Render/RHI/Buffer.h"

namespace Zero
{
	class FRGResourcePool
	{
		struct FPooledTexture2D
		{
			Scope<FTexture2D> Texture;
			uint64_t LastUsedFrame;
		};

		struct FPooledTextureCube
		{
			Scope<FTextureCube> Texture;
			uint64_t LastUsedFrame;
		};

		struct FPooledBuffer
		{
			Ref<FBuffer> Buffer;
			uint64_t LastUsedFrame;
		};

		struct FPoolRenderTarget2D
		{
			Ref<FRenderTarget2D> RenderTarget;
			uint64_t LastUsedFrame;
		};

		struct FPoolRenderTargetCube
		{
			Ref<FRenderTargetCube> RenderTarget;
			uint64_t LastUsedFrame;
		};

	public:
		FRGResourcePool() = default;
		void Tick();
		FTexture2D* AllocateTexture2D(const FTextureDesc& Desc, char const* Name);
		FTextureCube* AllocateTextureCube(const FTextureDesc& Desc, char const* Name);
		FBuffer* AllocateBuffer(const FBufferDesc& Desc);
		FRenderTarget2D* AllocateRenderTarget2D();
		FRenderTargetCube* AllocateRenderTargetCube();
		void ReleaseTexture2D(FTexture2D* Texture);
		void ReleaseTextureCube(FTextureCube* Texture);
		void ReleaseBuffer(FBuffer* Buffer);
		void ReleaseRenderTarget2D(FRenderTarget2D* RenderTarget);
		void ReleaseRenderTargetCube(FRenderTargetCube* RenderTarget);
	private:
		uint64_t m_FrameIndex = 0;
		std::vector<std::pair<FPooledTexture2D, bool>> m_Texture2DPool;
		std::vector<std::pair<FPooledTextureCube, bool>> m_TextureCubePool;
		std::vector<std::pair<FPooledBuffer, bool>> m_BufferPool;
		std::vector<std::pair<FPoolRenderTarget2D, bool>> m_RenderTarget2DPool;
		std::vector<std::pair<FPoolRenderTargetCube, bool>> m_RenderTargetCubePool;
	};
}