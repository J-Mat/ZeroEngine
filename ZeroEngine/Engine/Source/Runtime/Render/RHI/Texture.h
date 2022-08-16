#pragma once
#include "Core.h"

namespace Zero
{
	class ITexture
	{
	public:
		virtual ~ITexture() = default;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual void Bind(uint32_t Slot) const = 0;
	};
	
	class ITexture2D : ITexture
	{
	public:
		enum class Format
		{
			R8G8B8,
			R8G8B8A8,
			R32G32B32A32,
			R24G8,
		};
		virtual ~ITexture2D() = default;
	};
}