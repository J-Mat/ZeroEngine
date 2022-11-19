#pragma once
#include "Core/Base/PublicSingleton.h"
#include "Base/ClassDetailsMapping.h"
#include "Base/VariableDetailsMapping.h"

namespace Zero
{
	class FDetailMappingManager : public IPublicSingleton<FDetailMappingManager>
	{
	public:
		void RegisterVariableMapping(std::string PropertyType, Ref<FVariableDetailsMapping> PropertyDetailsMapping);
		void RegisterClassMapping(Ref<FClassDetailsMapping> ActorDetailsMapping);
		Ref<FVariableDetailsMapping> FindPropertyMapping(std::string PrpertyType);
		
		bool UpdateClassWidgets(UCoreObject* CoreObject);

	private:
		std::map<std::string, Ref<FVariableDetailsMapping>> m_VariableMapping;
		Ref<FClassDetailsMapping> m_ActorDetailsMapping;
		UProperty* m_CurrentProperty = nullptr;
	};
	
}