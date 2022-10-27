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
		Ref<FVariableDetailsMapping> FindPropertyMapping(std::string PrpertyType);
		
		bool UpdatePropertyWidgets(const std::string& Category, UProperty * Property);
		bool UpdateClassWidgets(UCoreObject* CoreObject);

	private:
		std::map<std::string, Ref<FVariableDetailsMapping>> m_VariableMapping;
		UProperty* m_CurrentProperty = nullptr;
	};
	
}