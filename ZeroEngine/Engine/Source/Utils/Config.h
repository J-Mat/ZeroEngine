#pragma once

#include <filesystem>

// version variable that will be substituted by cmake
// This shows an example using the @ variable type

namespace Zero
{
	namespace Config
	{
		static const std::filesystem::path EngineSourceDir = "D:/Toy/ZeroEngine/ZeroEngine/Engine/Source";
		static const std::filesystem::path ConfigDir = "D:/Toy/ZeroEngine/ZeroEngine/Config";
		static const std::string& ZBTFile = "ZBTConfig.ini";
		static const std::filesystem::path AssetsDir = "D:/Toy/ZeroEngine/ZeroEngine/Assets";
		static const std::filesystem::path IntermediateDir = "D:/Toy/ZeroEngine/ZeroEngine/Intermediate";
		static const std::filesystem::path CodeReflectionLinkFile = "D:/Toy/ZeroEngine/ZeroEngine/Engine/Source/CodeReflectionLinkFile.h";
	}
}
