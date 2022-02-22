#include "../../public/simple_image/simple_image.h"
#include "../../public/simple_image/Core/simple_image_bmp.h"
#include "../../public/simple_core_minimal/simple_c_helper_file/simple_file_helper.h"

namespace SimpleImage
{
	bool SaveBmpToDisk(const FBmpImageInfo& InImageInfo, const std::wstring& InSavePath, const std::vector<unsigned char>& InData)
	{
		if (InImageInfo.BmpHearder.bfSize < (sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)))
		{
			return false;
		}

		const wchar_t* InPath = InSavePath.c_str();

		std::vector<char> Data;
		Data.resize(InImageInfo.BmpHearder.bfSize);

		//初始化
		memset(Data.data(),0, InImageInfo.BmpHearder.bfSize);

		char *IndexPtr = Data.data();

		//指针的偏移操作
		{
			memcpy(IndexPtr,&InImageInfo.BmpHearder,sizeof(BITMAPFILEHEADER));

			IndexPtr += sizeof(BITMAPFILEHEADER);
			memcpy(IndexPtr, &InImageInfo.BmpInFoHeader, sizeof(BITMAPINFOHEADER));
			
			IndexPtr += sizeof(BITMAPINFOHEADER);
			if (InImageInfo.BmpInFoHeader.biBitCount == 8)
			{
				memcpy(IndexPtr, &InImageInfo.BmpColors, sizeof(InImageInfo.BmpColors));
				IndexPtr += sizeof(InImageInfo.BmpColors);
			}

			memcpy(IndexPtr, InData.data(), InData.size() * sizeof(unsigned char));		
		}

		//存储我们的数据
		return save_data_to_disk_w(InPath,Data.data(), Data.size());
	}

	bool GetRGBImageDataExcludeHeaderInfor(int InWidth, int InHeight, std::vector<unsigned char>& InData)
	{
		SimpleImage::FImageInfo ImageInfo;
		ImageInfo.Height = InWidth;
		ImageInfo.Width = InHeight;
		ImageInfo.Channel = SimpleImage::EImageChannel::Channel_24Bit;
		ImageInfo.ImageType = SimpleImage::EImageType::Bmp;

		return GetImageDataExcludeHeaderInfor(ImageInfo, InData);
	}

	bool GetImageDataExcludeHeaderInfor(const FImageInfo& InImageInfo, std::vector<unsigned char>& InData)
	{
		InData.resize(InImageInfo.Height * InImageInfo.Width * (int)InImageInfo.Channel);
	
		return InData.size() > (int)InImageInfo.Channel;
	}

	bool SaveImageToDisk(const FImageInfo& InImageInfo, const std::wstring& InSavePath, const std::vector<unsigned char>& InData)
	{
		switch (InImageInfo.ImageType)
		{
			case EImageType::Bmp:
			{
				FBmpImageInfo BmpImageInfo;

				//Hearder
				int ColorTableSize = 0;
				if (InImageInfo.Channel == EImageChannel::Channel_8Bit)
				{
					ColorTableSize = sizeof(BmpImageInfo.BmpColors);//1024

					BmpImageInfo.BmpInFoHeader.biBitCount = 8;

					BmpImageInfo.BmpInFoHeader.biClrUsed = 256;
					BmpImageInfo.BmpInFoHeader.biClrImportant = 256;

					//填充颜色表
					for (size_t i = 0; i < 256; i++)
					{
						BmpImageInfo.BmpColors[i].rgbBlue = i;
						BmpImageInfo.BmpColors[i].rgbGreen = i;
						BmpImageInfo.BmpColors[i].rgbRed = i;
						BmpImageInfo.BmpColors[i].rgbReserved = 0;
					}
				}
				else if (InImageInfo.Channel == EImageChannel::Channel_24Bit)
				{
					BmpImageInfo.BmpInFoHeader.biBitCount = 24;

					BmpImageInfo.BmpInFoHeader.biClrUsed = 0;
					BmpImageInfo.BmpInFoHeader.biClrImportant = 0;
				}

				BmpImageInfo.BmpHearder.bfType = 0x4D42;//bmp类型
				BmpImageInfo.BmpHearder.bfSize =
					sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 
					ColorTableSize + InImageInfo.Height * InImageInfo.Width * 3;
				BmpImageInfo.BmpHearder.bfReserved1 = 0;
				BmpImageInfo.BmpHearder.bfReserved2 = 0;
				BmpImageInfo.BmpHearder.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + ColorTableSize;

				//Info
				BmpImageInfo.BmpInFoHeader.biSize = sizeof(BITMAPINFOHEADER);
				BmpImageInfo.BmpInFoHeader.biWidth = InImageInfo.Width;
				BmpImageInfo.BmpInFoHeader.biHeight = InImageInfo.Height;
				BmpImageInfo.BmpInFoHeader.biPlanes = 1;
				BmpImageInfo.BmpInFoHeader.biCompression = 0;
				BmpImageInfo.BmpInFoHeader.biSizeImage = InImageInfo.Width * InImageInfo.Height * (int)InImageInfo.Channel;
				BmpImageInfo.BmpInFoHeader.biXPelsPerMeter = 0;
				BmpImageInfo.BmpInFoHeader.biYPelsPerMeter = 0;

				return SaveBmpToDisk(BmpImageInfo, InSavePath, InData);
			}
			case EImageType::Png:
			{
				break;
			}
			case EImageType::Jpg:
			{
				break;
			}
		}

		return false;
	}

	bool LoadImageToDisk(EImageType ImageType, const std::wstring& InLoadPath, std::vector<unsigned char>& OutData, FImageInfo* OutInfo)
	{
		const wchar_t *WPath = InLoadPath.c_str();
		
		//拿到文件大小
		unsigned int Len = get_file_size_by_filename_w(WPath);

		//读取数据
		std::vector<unsigned char> LoadData;
		LoadData.resize(Len);
		
		//读取文件
		load_data_from_disk_w(WPath,(char*)LoadData.data());

		switch (ImageType)
		{
			case SimpleImage::Bmp:
			{
				unsigned char *IndexPtr = LoadData.data();

				FBmpImageInfo BmpImageInfo;
				BmpImageInfo.BmpHearder = *(BITMAPFILEHEADER*)IndexPtr;

				//保证是bmp文件
				if (BmpImageInfo.BmpHearder.bfType == 0x4D42)
				{
					IndexPtr += sizeof(BITMAPFILEHEADER);
					BmpImageInfo.BmpInFoHeader = *(BITMAPINFOHEADER*)IndexPtr;

					//获取图片高宽
					OutInfo->Height = BmpImageInfo.BmpInFoHeader.biHeight;
					OutInfo->Width = BmpImageInfo.BmpInFoHeader.biWidth;

					OutInfo->ImageType = SimpleImage::Bmp;

					//算通道
					OutInfo->Channel = (EImageChannel)(BmpImageInfo.BmpInFoHeader.biBitCount / 8);
			
					//根据通道做判定
					switch (OutInfo->Channel)
					{
						case EImageChannel::Channel_8Bit:
						{
							memcpy(BmpImageInfo.BmpColors,IndexPtr,sizeof(BmpImageInfo.BmpColors));
							IndexPtr += sizeof(BmpImageInfo.BmpColors);
							break;
						}
						case EImageChannel::Channel_24Bit:
						{

							break;
						}
					}

					//计算颜色内容
					OutData.resize(BmpImageInfo.BmpInFoHeader.biSizeImage);
					memcpy(OutData.data(), IndexPtr, BmpImageInfo.BmpInFoHeader.biSizeImage);
			
					return true;
				}	
			}
			case SimpleImage::Png:
				break;
			case SimpleImage::Jpg:
				break;
		}

		return false;
	}
}