#include "WorldSerializer.h"
#include "Utils/StringUtils.h"
#include "SerializeUtility.h"
#include "World/Base/ObjectGlobal.h"

namespace Zero
{

	std::set<std::string> FWorldSerializer::s_AllSimpleType =
	{
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

	void FWorldSerializer::ImportValue(YAML::Emitter& Emitter, UProperty* Property)
	{
		Emitter << YAML::Key << "Type" << YAML::Value << Property->GetPropertyType();
		const std::string& Type = Property->GetPropertyType();
		if (Type == "int")
			Emitter << YAML::Key << "Value" << YAML::Value << *Property->GetData<int>();
		else if (Type == "string" || Type == "std::string")
			Emitter << YAML::Key << "Value" << YAML::Value << *Property->GetData<std::string>();
		else if (Type == "uint32_t")
			Emitter << YAML::Key << "Value" << YAML::Value << *Property->GetData<uint32_t>();
		else if (Type == "int32_t")
			Emitter << YAML::Key << "Value" << YAML::Value << *Property->GetData<int32_t>();
		else if (Type == "ZMath::vec3")
			Emitter << YAML::Key << "Value" << YAML::Value << *Property->GetData<ZMath::vec3>();
		else if (Type == "ZMath::vec4")
			Emitter << YAML::Key << "Value" << YAML::Value << *Property->GetData<ZMath::vec4>();
		else if (Type == "ZMath::FColor")
			Emitter << YAML::Key << "Value" << YAML::Value << *Property->GetData<ZMath::FColor>();
		else if (Type == "ZMath::FRotation")
			Emitter << YAML::Key << "Value" << YAML::Value << *Property->GetData<ZMath::FRotation>();
		else if (Type == "float")
			Emitter << YAML::Key << "Value" << YAML::Value << *Property->GetData<float>();
		else if (Type == "double")
			Emitter << YAML::Key << "Value" << YAML::Value << *Property->GetData<double>();
	}

	void FWorldSerializer::SerializeProperties(YAML::Emitter& Emitter, const std::string& ClassName, UProperty* Property)
	{
		while (Property != nullptr)
		{
			std::string CurrentClassName = Property->GetBelongClassName();

			if (CurrentClassName != ClassName)
			{
				break;
			}

			if (s_AllSimpleType.contains(Property->GetPropertyType()))
			{
				Emitter << YAML::BeginMap;
				Emitter << YAML::Key << "PropertyName" << YAML::Value << Property->GetPropertyName(false);
				ImportValue(Emitter, Property);
				Emitter << YAML::EndMap;
			}
			else
			{
				Emitter << YAML::BeginMap;
				Emitter << YAML::Key << "PropertyName" << YAML::Value << Property->GetPropertyName(false);
				Emitter << YAML::Key << "Type" << YAML::Value << Property->GetPropertyType();
				UCoreObject* Object = Property->GetData<UCoreObject>();
				SerializeCoreObject(Emitter, Object);
				Emitter << YAML::EndMap;
			}
			m_CurrentProperty = Property = dynamic_cast<UProperty*>(Property->Next);
		}
	}

	void FWorldSerializer::SerializeCoreObject(YAML::Emitter& Emitter, UCoreObject* CoreObject)
	{
		m_CurrentProperty = CoreObject->GetClassCollection().HeadProperty;

		std::list<std::string> InheritLink;
		std::string ClassName = CoreObject->GetObjectName();
		std::string DerivedClassName;
		for (auto Iter = GetClassInfoMap().find(ClassName);
			Iter != GetClassInfoMap().end();
			Iter = Iter = GetClassInfoMap().find(ClassName)
			)
		{
			InheritLink.push_front(ClassName);
			ClassName = Iter->second.DerivedClassName;
		}


		Emitter << YAML::Key << "DerivedLink" << YAML::Value << YAML::BeginSeq;
		for (auto& ClassName : InheritLink)
		{
			Emitter << YAML::BeginMap;
			Emitter << YAML::Key << "ClassName" << YAML::Value << ClassName;
			if (m_CurrentProperty == nullptr)
			{
				break;
			}
			const std::string& CurrentClassName = m_CurrentProperty->GetBelongClassName();
			Emitter << YAML::Key << "Properties" << YAML::Value << YAML::BeginSeq;
			if (CurrentClassName == ClassName)
			{
				SerializeProperties(Emitter, ClassName, m_CurrentProperty);
			}
			Emitter << YAML::EndSeq;
			Emitter << YAML::EndMap;
		}
		Emitter << YAML::EndSeq;
	}

	void FWorldSerializer::Serialize(const std::filesystem::path Path)
	{
		YAML::Emitter Emitter;
		Emitter << YAML::BeginMap;
		
		Emitter << YAML::Key << "Scene" << YAML::Value << "Untitled";
		Emitter << YAML::Key << "Actors" << YAML::Value << YAML::BeginSeq;
		
		std::vector<UActor*> Actors = m_World->GetActors();
		for (int i = 0; i < Actors.size(); ++i)
		{
			UActor* Actor = Actors[i];
			Emitter << YAML::BeginMap;
			Emitter << YAML::Key << "ActorName" << YAML::Value << Actor->GetObjectName();
			SerializeCoreObject(Emitter, Actor);
			Emitter << YAML::EndMap;
		}
		
		Emitter << YAML::EndSeq;

		Emitter << YAML::EndMap;
		Utils::StringUtils::WriteFile(Path.string(), Emitter.c_str());
	}
	
}
