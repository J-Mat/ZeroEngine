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
		void SerializeProperties(YAML::Emitter& Out, const std::string& ClassName, UProperty* CurrentProperty);
		void SerializeCoreObject(YAML::Emitter& Out, UCoreObject* Actor);
		void Serialize(const std::filesystem::path Path);
		
		void Deserialize(const std::filesystem::path Path);
	private:
		static std::set<std::string> s_AllSimpleType;
		UWorld* m_World;
		UProperty* m_CurrentProperty;
	};
}
