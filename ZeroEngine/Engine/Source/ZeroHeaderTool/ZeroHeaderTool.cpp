#include <iostream> 
#include "StringUtils.h"
#include "ZConfig.h"
#include "Log.h"
#include "mini/ini.h"
#include "CppParser.h"
#include "Utils.h"


ZHT::FFileParser g_FileParser;
std::set<std::filesystem::path> g_AllLinkCppFiles;


void ParseDir(const std::filesystem::path& Folder)
{
	for (auto& Child : std::filesystem::directory_iterator(Folder))
	{
		if (std::filesystem::is_directory(Child))
		{
			ParseDir(Child.path());
		}
		else if (Child.path().extension().string() == ".h")
		{
			g_FileParser.Parse(Child.path());
			if (g_FileParser.CheckNeedGenerateReflection())
			{ 
				ZHT::FClassElement ClassElement;
				g_FileParser.CollectClassInfo(ClassElement);
				g_FileParser.LogClassInfo(ClassElement);
				g_FileParser.GenerateReflectionHeaderFile(ClassElement);
				g_FileParser.GenerateReflectionCppFile(ClassElement);
				g_AllLinkCppFiles.insert(ClassElement.CppPath);
			}
		}
	}
}


int main()
{
	Zero::FLog::Init();
	std::filesystem::path Path = Zero::ZConfig::ConfigDir / Zero::ZConfig::ZBTFile;
	// Removeo all files first
	std::cout << Path.string() << std::endl;
	mINI::INIFile File(Path.string());
	mINI::INIStructure Ini;
	File.read(Ini);
	
	auto Modules  = Ini["Modules"];;

	for (auto const& Iter : Modules)
	{
		//std::cout << Iter.second << std::endl;
		std::filesystem::path PathModule = Zero::ZConfig::EngineSourceDir / Iter.second;
		CLIENT_LOG_INFO("Parse Module {0}", PathModule.string());
		if (std::filesystem::exists(PathModule))
		{
			ParseDir(PathModule);
		}
	}
	

	g_FileParser.WriteLinkReflectionFile(g_AllLinkCppFiles);
	//file.generate(ini);
	return 0;
}