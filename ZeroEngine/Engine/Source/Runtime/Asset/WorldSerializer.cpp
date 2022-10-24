#include "WorldSerializer.h"
#include "Utils/StringUtils.h"

namespace Zero
{
	FWorldSerializer::FWorldSerializer(UWorld* World)
		: m_World(World)
	{
	}

	void FWorldSerializer::Serialize(const std::filesystem::path Path)
	{
		YAML::Emitter Out;
		Out << YAML::BeginMap;
		
		Out << YAML::Key << "Scene" << YAML::Value << "Untitled";
		Out << YAML::Key << "Actors" << YAML::Value << YAML::BeginSeq;
		
		std::vector<UActor*> Actors = m_World->GetActors();
		for (int i = 0; i < Actors.size(); ++i)
		{
			UActor* Actor = Actors[i];
			Out << YAML::BeginMap;
			
			Out << YAML::EndMap;
		}
		
		Out << YAML::EndSeq;

		Out << YAML::EndMap;
		Utils::StringUtils::WriteFile(Path.string(), Out.c_str());
	}
	
}
