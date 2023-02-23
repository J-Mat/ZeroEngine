#include "Image.h"
#include "stb_image.h"

namespace Zero
{
	FImage::FImage(std::string Path)
	{
		m_Channels = 4;
		int ChanelInFile;
		auto stb_data = stbi_load(Path.c_str(), &m_Width, &m_Height, &ChanelInFile, m_Channels);
		CORE_ASSERT(stb_data, "Image Loaded from Path Falied!");
		if (m_Channels == 3)
		{
			m_Type = EImgeType::RGB;
		}
		else if (m_Channels == 4)
		{
			m_Type = EImgeType::RGBA;
		}
		m_Data = new unsigned char[m_Width * m_Height * m_Channels];
		for (size_t i = 0; i < m_Width * m_Height * m_Channels; i++)
			m_Data[i] = stb_data[i];
		m_BufferSize = m_Width * m_Height * m_Channels * sizeof(unsigned char);
		stbi_image_free(stb_data);
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