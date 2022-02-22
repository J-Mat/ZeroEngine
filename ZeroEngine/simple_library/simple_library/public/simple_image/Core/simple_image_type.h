#pragma once

namespace SimpleImage
{
	enum EImageType
	{
		Bmp,
		Png,
		Jpg,
		Tag
	};

	enum EImageChannel
	{
		Channel_8Bit = 1,
		Channel_24Bit = 3,
	};

	struct FImageInfo
	{
		FImageInfo()
			:ImageType(EImageType::Bmp)
			, Width(0)
			, Height(0)
			, Channel(EImageChannel::Channel_24Bit)
		{}


		EImageType ImageType;
		int Width;
		int Height;

		EImageChannel Channel;
	};
}