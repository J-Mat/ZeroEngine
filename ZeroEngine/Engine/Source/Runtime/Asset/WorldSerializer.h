#pragma once

#include "yaml-cpp/yaml.h"
#include "World/World.h"

namespace Zero
{
	class FWorldSerializer
	{
	public:
		FWorldSerializer(UWorld* World);
		void Serialize(const std::filesystem::path Path);
		
	private:
		UWorld* m_World;
	};
}
