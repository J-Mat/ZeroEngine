#pragma once

#include "Core/Framework/Application.h"

namespace Zero
{
	class FFileDialog
	{
	public:
		static std::string OpenFile(const char* Filter, const char* Extension);
		static std::string SaveFile(const char* Filter, const char* Extension);
	};
}