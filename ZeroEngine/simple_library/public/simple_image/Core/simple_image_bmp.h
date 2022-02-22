#pragma once
#include "../../../public/simple_core_minimal/simple_c_core/simple_core_minimal.h"
#include "../../../public/simple_cpp_core_minimal/simple_cpp_core_minimal.h"

namespace SimpleImage
{
	struct FBmpImageInfo
	{
		BITMAPFILEHEADER BmpHearder;		//ͷ��Ϣ
		BITMAPINFOHEADER BmpInFoHeader;		//ͷ��Ϣ2
		RGBQUAD          BmpColors[256];	//��ɫ��
	};
}