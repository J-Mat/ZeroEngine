#include "WorldSerializer.h"
#include "Utils/StringUtils.h"
#include "SerializeUtility.h"
#include "World/Base/ObjectGlobal.h"
#include "InitActorGeneratedFile.h"

namespace Zero
{

	std::set<std::string> FWorldSerializer::s_AllSimpleType =
	{
		"bool",
		"int",  
		"std::string",
		"string",
		"uint32_t",
		"int32_t",
		"ZMath::vec3",
		"ZMath::vec4",
		"ZMath::FColor",
		"ZMath::FRotation",
		"float",
		"double"
	};

	FWorldSerializer::FWorldSerializer(UWorld* World)
		: m_World(World)
	{
	}

	void FWorldSerializer::ImportValue(YAML::Emitter& Out, UProperty* Property)
	{
		Out << YAML::Key << "Type" << YAML::Value << Property->GetPropertyType();
		const std::string& Type = Property->GetPropertyType();
		std::cout << "type:" << Type << std::endl;
		if (Type == "bool")
			Out << YAML::Key << "Value" << YAML::Value << *Property->GetData<bool>();
		else if (Type == "int")
			Out << YAML::Key << "Value" << YAML::Value << *Property->GetData<int>();
		else if (Type == "string" || Type == "std::string")
			Out << YAML::Key << "Value" << YAML::Value << *Property->GetData<std::string>();
		else if (Type == "uint32_t")
			Out << YAML::Key << "Value" << YAML::Value << *Property->GetData<uint32_t>();
		else if (Type == "int32_t")
			Out << YAML::Key << "Value" << YAML::Value << *Property->GetData<int32_t>();
		else if (Type == "ZMath::vec3")
			Out << YAML::Key << "Value" << YAML::Value << *Property->GetData<ZMath::vec3>();
		else if (Type == "ZMath::vec4")
			Out << YAML::Key << "Value" << YAML::Value << *Property->GetData<ZMath::vec4>();
		else if (Type == "ZMath::FColor")
			Out << YAML::Key << "Value" << YAML::Value << *Property->GetData<ZMath::FColor>();
		else if (Type == "ZMath::FRotation")
			Out << YAML::Key << "Value" << YAML::Value << *Property->GetData<ZMath::FRotation>();
		else if (Type == "float")
			Out << YAML::Key << "Value" << YAML::Value << *Property->GetData<float>();
		else if (Type == "double")
			Out << YAML::Key << "Value" << YAML::Value << *Property->GetData<double>();
	}

	void FWorldSerializer::ExporttValue(YAML::Node& Data, UProperty* Property)
	{
		std::string PropertyNameWithClassName = std::format("{0}:{1}", Property->GetPropertyName(), Property->GetBelongClassName());
		YAML::Node PropertyNode = Data[PropertyNameWithClassName];
		const std::string& Type = Property->GetPropertyType();
		void* PropertyData = Property->GetData();
		if (PropertyNode["Type"].as<std::string>() != Type)
		{
			return;
		}
		if (Type == "bool")
			*((bool*)PropertyData) = PropertyNode["Value"].as<bool>();
		else if (Type == "int")
			*((int*)PropertyData) = PropertyNode["Value"].as<int>();
		else if (Type == "string" || Type == "std::string")
			*((std::string*)PropertyData) = PropertyNode["Value"].as<std::string>();
		else if (Type == "uint32_t")
			*((uint32_t*)PropertyData) = PropertyNode["Value"].as<uint32_t>();
		else if (Type == "int32_t")
			*((int32_t*)PropertyData) = PropertyNode["Value"].as<int32_t>();
		else if (Type == "ZMath::vec3")
			*((ZMath::vec3*)PropertyData) = PropertyNode["Value"].as<ZMath::vec3>();
		else if (Type == "ZMath::vec4")
			*((ZMath::vec4*)PropertyData) = PropertyNode["Value"].as<ZMath::vec4>();
		else if (Type == "ZMath::FColor")
			*((ZMath::FColor*)PropertyData) = PropertyNode["Value"].as<ZMath::FColor>();
		else if (Type == "ZMath::FRotation")
			*((ZMath::FRotation*)PropertyData) = PropertyNode["Value"].as<ZMath::FRotation>();
		else if (Type == "float")
			*((float*)PropertyData) = PropertyNode["Value"].as<float>();
		else if (Type == "double")
			*((double*)PropertyData) = PropertyNode["Value"].as<double>();
	}


	void FWorldSerializer::SerializeVariableProperties(YAML::Emitter& Out, UCoreObject* CoreObject)
	{
		UProperty* Property = CoreObject->GetClassCollection().HeadProperty;

		Out << YAML::Key << "Properties" << YAML::Value << YAML::BeginMap;
		while (Property != nullptr)
		{
			if (s_AllSimpleType.contains(Property->GetPropertyType()))
			{
				std::string PropertyNameWithClassName = std::format("{0}:{1}", Property->GetPropertyName(), Property->GetBelongClassName());
				Out << YAML::Key << PropertyNameWithClassName << YAML::Value << YAML::BeginMap;
				ImportValue(Out, Property);
				Out << YAML::EndMap;
			}
			Property = Property = dynamic_cast<UProperty*>(Property->Next);
		}
		Out << YAML::EndMap;
	}

	void FWorldSerializer::SerializeComponent(YAML::Emitter& Out, UComponent* Componnet)
	{
		Out << YAML::BeginMap;
		Out << YAML::Key << "ComopnentName" << YAML::Value << Componnet->GetObjectName();
		Out << YAML::Key << "Properties" << YAML::Value << YAML::BeginMap;
		UProperty* Property = Componnet->GetClassCollection().HeadProperty;
		while (Property != nullptr)
		{
			if (s_AllSimpleType.contains(Property->GetPropertyType()))
			{
				std::string PropertyNameWithClassName = std::format("{0}:{1}", Property->GetPropertyName(), Property->GetBelongClassName());
				Out << YAML::Key << PropertyNameWithClassName << YAML::Value << YAML::BeginMap;
				ImportValue(Out, Property);
				Out << YAML::EndMap;
			}
			Property = Property = dynamic_cast<UProperty*>(Property->Next);
		}
		Out << YAML::EndMap;
		Out << YAML::EndMap;
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
			UActor* Actor = Actors[i];
			Out << YAML::BeginMap;
			Out << YAML::Key << "ActorName" << YAML::Value << Actor->GetObjectName();
			SerializeVariableProperties(Out, Actor);
			
			Out << YAML::Key << "Component" << YAML::Value << YAML::BeginSeq;
	
			for (UComponent* Component : Actor->GetAllComponents())
			{
				SerializeComponent(Out, Component);
			}
			
			Out << YAML::EndSeq;

			Out << YAML::EndMap;
		}
		
		Out << YAML::EndSeq;

		Out << YAML::EndMap;
		Utils::StringUtils::WriteFile(Path.string(), Out.c_str());
	}


	void FWorldSerializer::DeserializeVariablePrperties(YAML::Node& Data, UCoreObject* CoreObject)
	{
		auto PropertiesNode = Data["Properties"];
		UProperty* Property = CoreObject->GetClassCollection().HeadProperty;
		while (Property != nullptr)
		{
			ExporttValue(PropertiesNode, Property);
			Property = Property = dynamic_cast<UProperty*>(Property->Next);
		}
	}

	bool FWorldSerializer::DeserializeComponents(YAML::Node& Data, UActor* Actor)
	{
		auto ComponnetNode = Data["Component"];
		auto NodeIter = ComponnetNode.begin();
		for (UComponent* Component : Actor->GetAllComponents())
		{
			if (Component->GetObjectName() != (*NodeIter)["ComopnentName"].as<std::string>())
			{
				return false;
			}
			DeserializeVariablePrperties(*NodeIter, Component);
			NodeIter++;
		}
		return true;
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
				auto Iter = GetClassInfoMap().find(ActorName);
				if (Iter != GetClassInfoMap().end())
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
					DeserializeVariablePrperties(ActorNode, Actor);
					if (!DeserializeComponents(ActorNode, Actor))
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
