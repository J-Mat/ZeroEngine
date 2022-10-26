#include "WorldSerializer.h"
#include "Utils/StringUtils.h"
#include "SerializeUtility.h"
#include "World/Base/ObjectGlobal.h"

namespace Zero
{
	FWorldSerializer::FWorldSerializer(UWorld* World)
		: m_World(World)
	{
	}

	void FWorldSerializer::SerializeProperties(YAML::Emitter& Out, const std::string& ClassName, UProperty* Property)
	{
		while (Property != nullptr)
		{
			std::string CurrentClassName = Property->GetBelongClassName();

			if (CurrentClassName != ClassName)
			{
				break;
			}

			Ref<FVariableDetailsMapping> VariableDetailsMapping = FindPropertyMapping(Property->GetPropertyType());
			if (VariableDetailsMapping != nullptr)
			{
				VariableDetailsMapping->UpdateDetailsWidget(Property);
			}
			else
			{
				UCoreObject* Object = Property->GetData<UCoreObject>();
				SerializeActor(Object);
			}
			m_CurrentProperty = Property = dynamic_cast<UProperty*>(Property->Next);
		}
		ImGui::Separator();
		return false;
	}

	void FWorldSerializer::SerializeActor(YAML::Emitter& Out, UCoreObject* CoreObject)
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

		for (auto& ClassName : InheritLink)
		{
			if (m_CurrentProperty == nullptr)
			{
				break;
			}
			const std::string& CurrentClassName = m_CurrentProperty->GetBelongClassName();
			if (CurrentClassName == ClassName)
			{
				SerializeProperties(Out, ClassName, m_CurrentProperty);
			}
		}
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
			Out << YAML::Key << "ObjName" << YAML::Value << Actor->GetObjectName();
			SerializeActor(Out, Actor);
			Out << YAML::EndMap;
		}
		
		Out << YAML::EndSeq;

		Out << YAML::EndMap;
		Utils::StringUtils::WriteFile(Path.string(), Out.c_str());
	}
	
}
