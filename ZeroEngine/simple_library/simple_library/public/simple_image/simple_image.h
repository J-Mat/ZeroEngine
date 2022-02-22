#pragma once
#include "../simple_cpp_core_minimal/simple_cpp_core_minimal.h"
#include "Core/simple_image_type.h"

namespace SimpleImage
{
	bool GetRGBImageDataExcludeHeaderInfor(int InWidth,int InHeight, std::vector<unsigned char>& InData);
	bool GetImageDataExcludeHeaderInfor(const FImageInfo& InImageInfo,std::vector<unsigned char> &InData);

	bool SaveImageToDisk(const FImageInfo &InImageInfo, const std::wstring& InSavePath, const std::vector<unsigned char> &InData);
	bool LoadImageToDisk(EImageType ImageType, const std::wstring& InLoadPath, std::vector<unsigned char> &OutData,FImageInfo *OutInfo = NULL);
}