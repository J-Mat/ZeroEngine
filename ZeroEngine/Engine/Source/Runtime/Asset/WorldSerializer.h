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
		void Serialize(const std::filesystem::path Path);
		bool Deserialize(const std::filesystem::path Path);
	private:
		UWorld* m_World;
	};
}
