#include <iostream> 
#include "StringUtils.h"
#include "Config.h"
#include "Log.h"
#include "mini/ini.h"
#include "CppParser.h"


ZHT::FFileParser g_FileParser;


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
			CLIENT_LOG_INFO(Child.path().string());
			g_FileParser.Parse(Child.path());
			if (g_FileParser.CheckNeedGenerateReflection())
			{ 
				ZHT::FClassElement ClassElement;
				g_FileParser.GenerateCodeReflectionFile();
			}
		}
	}
}


int main()
{
	Zero::FLog::Init();
	std::filesystem::path Path = Zero::Config::ConfigDir / Zero::Config::ZBTFile;
	std::cout << Path.string() << std::endl;
	mINI::INIFile File(Path.string());
	mINI::INIStructure Ini;
	File.read(Ini);
	
	auto Modules  = Ini["Modules"];;

	for (auto const& Iter : Modules)
	{
		//std::cout << Iter.second << std::endl;
		std::filesystem::path PathModule = Zero::Config::EngineSourceDir / Iter.second;
		CLIENT_LOG_INFO("Parse Module {0}", PathModule.string());
		if (std::filesystem::exists(PathModule))
		{
			ParseDir(PathModule);
		}
	}
	
	//file.generate(ini);
	return 0;
}