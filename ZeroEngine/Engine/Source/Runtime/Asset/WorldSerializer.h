#pragma once

#include "yaml-cpp/yaml.h"
#include "World/World.h"
#include "World/Base/PropertyObject.h"
#include "Delegate.h"


namespace Zero
{
	DEFINITION_SIMPLE_SINGLE_DELEGATE(FReturnBool, bool, UProperty);

	class FWorldSerializer
	{
	public:
		FWorldSerializer(UWorld* World);
		void ImportValue(YAML::Emitter& Out, UProperty* CurrentProperty);
		void ExporttValue(YAML::Node& Data, UProperty* CurrentProperty);
		void SerializeVariableProperties(YAML::Emitter& Out, UCoreObject* Actor);
		void SerializeComponent(YAML::Emitter& Out, UComponent* Componnet);
		void Serialize(const std::filesystem::path Path);
		
		void DeserializeVariablePrperties(YAML::Node& Data, UCoreObject* CoreObject);
		bool DeserializeComponents(YAML::Node& Data, UActor* Actor);
		bool Deserialize(const std::filesystem::path Path);
	private:
		static std::set<std::string> s_AllSimpleType;
		UWorld* m_World;
		UProperty* m_CurrentProperty;
	};
}
