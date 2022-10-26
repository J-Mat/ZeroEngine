#pragma once

#include "yaml-cpp/yaml.h"
#include "World/World.h"
#include "World/Base/PropertyObject.h"

namespace Zero
{
	class FWorldSerializer
	{
	public:
		FWorldSerializer(UWorld* World);
		void SerializeProperties(YAML::Emitter& Out, const std::string& ClassName, UProperty* m_CurrentProperty);
		void SerializeActor(YAML::Emitter& Out, UCoreObject* Actor);
		void Serialize(const std::filesystem::path Path);
		
	private:
		UWorld* m_World;
		UProperty* m_CurrentProperty;
	};
}
