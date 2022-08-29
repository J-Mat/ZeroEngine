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
		uint32_t m_Width;
		uint32_t m_Height;
	}; 

	class FTexture2D : ITexture
	{
	public:
		enum class Format
		{
			R8G8B8,
			R8G8B8A8,
			R32G32B32A32,
			R24G8,
		};
		virtual ~FTexture2D() = default;
		virtual void Resize(uint32_t Width, uint32_t Height, uint32_t DepthOrArraySize = 1) = 0;
		virtual ZMath::uvec2 GetSize() = 0;
	};
}