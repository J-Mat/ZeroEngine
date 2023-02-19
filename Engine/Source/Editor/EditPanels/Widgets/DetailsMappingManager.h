#pragma once
#include "Core/Base/PublicSingleton.h"
#include "Base/ClassDetailsMapping.h"
#include "Base/VariableDetailsMapping.h"

namespace Zero
{
	class FDetailMappingManager : public IPublicSingleton<FDetailMappingManager>
	{
	public:
		void RegisterVariableMapping(const std::string& PropertyType, Ref<FVariableDetailsMapping> PropertyDetailsMapping);
		void RegisterClassMapping(const std::string& ClassType, Ref<FClassDetailsMapping> ActorDetailsMapping);
		Ref<FVariableDetailsMapping> FindPropertyMapping(const std::string& PrpertyType);
		Ref<FClassDetailsMapping> FindClassMapping(const std::string& ClassType);
	private:
		std::map<std::string, Ref<FVariableDetailsMapping>> m_VariableMapping;
		std::map<std::string, Ref<FClassDetailsMapping>> m_ClassDetailsMapping;
		UProperty* m_CurrentProperty = nullptr;
	};
	
}