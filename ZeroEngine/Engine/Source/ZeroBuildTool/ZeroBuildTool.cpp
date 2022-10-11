#include <iostream> 
#include "StringUtils.h"
#include "Config.h"
#include "mini/ini.h"
int main()
{
	std::filesystem::path Path = Zero::Config::ConfigDir / Zero::Config::ZBTFile;
	std::cout << Path.string() << std::endl;
	mINI::INIFile File(Path.string());
	mINI::INIStructure Ini;
	File.read(Ini);
	
	auto Modules  = Ini["Modules"];;

	for (auto const& Iter : Modules)
	{
		std::cout << Iter.second << std::endl;
	}
	
	//file.generate(ini);
	return 0;
}