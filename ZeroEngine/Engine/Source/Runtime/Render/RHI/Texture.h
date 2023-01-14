#pragma once
#include "Core.h"

namespace Zero
{
	class ITexture
	{
	public:
		virtual ~ITexture() = default;
		virtual uint32_t GetWidth() { return m_Width; };
		virtual uint32_t GetHeight() { return m_Height; };
		virtual void Bind(uint32_t Slot) {};
	protected:
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
	}; 

	enum class ETextureFormat
	{
		None,
		R8G8B8,
		R8G8B8A8,
		INT32,
		R16G16B16A16,
		R32G32B32A32,
		DEPTH32F,
	};

	using FFrameBufferTexturesFormats = std::vector<ETextureFormat>;


	class FImage;
	class FTexture2D : public ITexture
	{
	public:
		FTexture2D(bool bNeetMipMap = false) :
			m_bNeedMipMap(bNeetMipMap) 
		{}
		virtual ~FTexture2D() = default;
		virtual Ref<FImage> GetImage() { return m_ImageData; };
		virtual void Resize(uint32_t Width, uint32_t Height, uint32_t DepthOrArraySize = 1) = 0;
		virtual ZMath::uvec2 GetSize() = 0;
		virtual void RegistGuiShaderResource() = 0;
		virtual UINT64 GetGuiShaderReseource() = 0;
	protected:	
        Ref<FImage> m_ImageData = nullptr;
		bool m_bNeedMipMap = false;
	};

	class FTextureCubemap : public ITexture
	{
	public:
		enum class Format
		{
			R8G8B8,
			R8G8B8A8,
			R32G32B32A32,
			R24G8,
		};
		virtual ~FTextureCubemap() = default;
		virtual void Resize(uint32_t Width, uint32_t Height, uint32_t DepthOrArraySize = 1) = 0;
		virtual ZMath::uvec2 GetSize() = 0;
	};
}