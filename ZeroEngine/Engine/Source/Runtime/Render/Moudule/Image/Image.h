#pragma once

#include "Core.h"

namespace Zero
{
	struct FRGBPixel
	{
		ZMath::vec3 RGB;
		FRGBPixel(float r, float g, float b)
		{
			RGB.r = r; RGB.g = g; RGB.b = b;
		}
	};

	struct FRGBAPixel
	{
		ZMath::vec4 RGBA;
		FRGBAPixel(float r, float g, float b, float a)
		{
			RGBA.r = r; RGBA.g = g; RGBA.b = b; RGBA.a = a;
		}
	};
	
	class FImage  
	{
	public:
		enum class EImgeType
		{	
			RGB,
			RGBA,
		};
		FImage(std::string Path);
		FImage(uint32_t Width, uint32_t Height, uint32_t Channels, ZMath::vec4 Color = {0,0,0,0 });
		~FImage();

		void Clear(ZMath::vec4 Color = { 0,0,0,0 });

		uint32_t GetWidth() { return m_Width; }
		uint32_t GetHeight() { return m_Height; }
		uint32_t GetChannel() { return m_Channels; }
		unsigned char* GetData() { return m_Data; }
		uint32_t GetBufferSize() { return m_BufferSize; }

	private:
		int m_Width;
		int m_Height;
		int m_Channels;
		EImgeType m_Type;

		unsigned char* m_Data;
		uint32_t m_BufferSize;
	};

}