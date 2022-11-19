#include "Image.h"
#include "stb_image.h"

namespace Zero
{
	FImage::FImage(std::string Path)
	{
		//stbi_set_flip_vertically_on_load(1);
		m_Data = stbi_load(Path.c_str(), &m_Width, &m_Height, &m_Channels, 0);
		CORE_ASSERT(m_Data, "Image Loaded from Path Falied!");
		if (m_Channels == 3)
		{
			m_Type = EImgeType::RGB;
		}
		else if (m_Channels == 4)
		{
			m_Type = EImgeType::RGBA;
		}
		m_BufferSize = m_Width * m_Height * m_Channels * sizeof(unsigned char);
	}
	FImage::FImage(uint32_t Width, uint32_t Height, uint32_t Channels, ZMath::vec4 Color)
		:m_Width(Width)
		,m_Height(Height)
		,m_Channels(Channels)
	{
		m_BufferSize = m_Width * m_Height * m_Channels * sizeof(unsigned char);
		m_Data = (unsigned char*)malloc(m_BufferSize);
		Clear(Color);
	}
	FImage::~FImage()
	{
		free(m_Data);
	}
	void FImage::Clear(ZMath::vec4 Color)
	{
		uint32_t ColFill = 0;
		ColFill += ((uint32_t)(uint8_t)(Color.a * 255)) << 24;
		ColFill += ((uint32_t)(uint8_t)(Color.b * 255)) << 16;
		ColFill += ((uint32_t)(uint8_t)(Color.g * 255)) << 8;
		ColFill += ((uint32_t)(uint8_t)(Color.r * 255));

		std::fill((uint32_t*)m_Data, (uint32_t*)(m_Data + m_BufferSize), ColFill);
	}
}