#include "WorldSerializer.h"
#include "Utils/StringUtils.h"
#include "SerializeUtility.h"
#include "World/Object/ObjectGlobal.h"

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
		
		Out << YAML::Key << "Scene" << YAML::Value << Path.filename().string().c_str();
		Out << YAML::Key << "Actors" << YAML::Value << YAML::BeginSeq;
		
		std::vector<UActor*> Actors = m_World->GetActors();
		for (int i = 0; i < Actors.size(); ++i)
		{
			UActor* Actor = Actors[i] ;
			Out << YAML::BeginMap;
			Out << YAML::Key << "ActorName" << YAML::Value << Actor->GetObjectName();
			FSerializeUtility::SerializeVariableProperties(Out, Actor);
			
			Out << YAML::Key << "Component" << YAML::Value << YAML::BeginSeq;
	
			for (UComponent* Component : Actor->GetAllComponents())
			{
				FSerializeUtility::SerializeComponent(Out, Component);
			}
			
			Out << YAML::EndSeq;

			Out << YAML::EndMap;
		}
		
		Out << YAML::EndSeq;

		Out << YAML::EndMap;
		Utils::StringUtils::WriteFile(Path.string(), Out.c_str());
	}

	bool FWorldSerializer::Deserialize(const std::filesystem::path Path)
	{
		YAML::Node Data;
		try
		{
			Data = YAML::LoadFile(Path.string());
		}
		catch (YAML::ParserException e)
		{
			return false;
		}
		m_World->SetSceneName(Data["Scene"].as<std::string>());
		CORE_LOG_TRACE("Deserializing scene '{0}'", m_World->GetSceneName());
		
		auto AllActorsNode = Data["Actors"];
		if (AllActorsNode)
		{ 
			for (auto ActorNode : AllActorsNode)
			{ 
				std::string ActorName = ActorNode["ActorName"].as<std::string>();
				CORE_LOG_TRACE("Actor Name '{0}'", ActorName);
				auto Iter = FObjectGlobal::GetClassInfoMap().find(ActorName);
				if (Iter != FObjectGlobal::GetClassInfoMap().end())
				{
					UActor* Actor = nullptr;
					if (ActorName != "UCameraActor")
					{
						Actor = static_cast<UActor*>(Iter->second.Class->CreateDefaultObject());
						m_World->AddActor(Actor);
					}
					else
					{
						Actor = m_World->GetMainCamera();
					}
					FSerializeUtility::DeserializeVariablePrperties(ActorNode, Actor);
					if (!FSerializeUtility::DeserializeComponents(ActorNode, Actor))
					{
						return false;
					}
					Actor->SetWorld(m_World);
					Actor->PostInit();
				}
			}
		}
		return true;
	}
}
