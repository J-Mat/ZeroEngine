#include <iostream> 
#include "StringUtils.h"
#include "ZConfig.h"
#include "Log.h"
#include "mini/ini.h"
#include "Utils.h"
#include "tinyxml2.h"
#include "json.hpp"
#include <string>
#include <stdio.h>
#include <fstream>

using FJsonObj = nlohmann::json;
using FImagePath = std::string;

class FMaterialConfig :public FJsonObj
{
public:
	FMaterialConfig() {};
	FMaterialConfig(const std::string& Str)
	{
		m_JsonObj = Str;
		
	}
private:
	void ParseJson() 
	{
		m_ShaderFile = m_JsonObj["ShaderFile"];
		FJsonObj TextureObj = m_JsonObj["Textures"];
	};
	std::string m_ShaderFile;
	std::map<std::string, FImagePath> m_Texture;
	FJsonObj m_JsonObj;
	std::string GetShaderFile() { return m_ShaderFile; }
	//FMaterialConfig& GetTextures() { return  (*this)["Textures"];};
	
};
int main()
{
	FJsonObj JsonExport;
	JsonExport["ShaderFile"] = "ForwardList.hlsl";
	FJsonObj& Textures = JsonExport["Textures"];
	Textures["gDiffuseMap"] = nullptr;
	Textures["gNormalMap"] = nullptr;
	
	FMaterialConfig Material = JsonExport.dump();

	//std::cout << Material.dump();
	std::cout << JsonExport.dump();
	std::cout << 11;
	

	return 0;
}